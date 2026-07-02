# citty Rendering Pipeline — A Complete Technical Reference

This document explains, from first principles, how a frame gets from the ECS world to
pixels on screen in `citty`. It covers the math (vectors, matrices, projection,
shading, tone mapping), how that math is encoded in the C++/Eigen and GLSL source,
the OpenGL object setup required to run it, and the exact memory layout of every
buffer involved. Every section references the real source files and line numbers so
you can cross-check the explanation against the implementation.

Engine is `citty/engine/*`. Renderer is `citty/graphics/*`. Shaders are `shaders/*`.

---

## Table of contents

1. [System overview](#1-system-overview)
2. [Mathematical foundations](#2-mathematical-foundations)
3. [ECS architecture and how it feeds the renderer](#3-ecs-architecture-and-how-it-feeds-the-renderer)
4. [OpenGL object setup](#4-opengl-object-setup)
5. [Memory layout](#5-memory-layout)
6. [The frame, pass by pass](#6-the-frame-pass-by-pass)
7. [Equation cheat sheet](#7-equation-cheat-sheet)

---

## 1. System overview

`citty` is a **tiled forward (a.k.a. "Forward+") renderer**. Unlike a naive forward
renderer (every fragment loops over every light) or a classic deferred renderer
(every fragment's full material is written to a G-buffer), Forward+ keeps a single
forward shading pass but first figures out, per screen-space tile, which lights can
possibly affect it. This is cheap in memory (no G-buffer) and scales to many lights
without an O(lights × fragments) cost.

The per-frame pipeline, driven by `RenderingEngine::render()`
(`citty/graphics/RenderingEngine.cpp:180`), is four GPU passes:

```
ECS world (entities + components)
        │
        ▼  GLFWRenderingSystem::update()  (citty/graphics/GLFWRenderingSystem.cpp:63)
GraphicsEntity[] / PointLightEntity[]   (flat, render-ready snapshots)
        │
        ▼  RenderingEngine::render()
┌─────────────────────────────────────────────────────────────────────┐
│ Pass 1 — Depth prepass        depth.vsh / depth.fsh                 │
│ Pass 2 — Light culling        lightCulling.csh   (compute shader)   │
│ Pass 3 — Light accumulation   lightAccumulation.vsh / .fsh          │
│ Pass 4 — HDR resolve          hdr.vsh / hdr.fsh                     │
└─────────────────────────────────────────────────────────────────────┘
        │
        ▼
default framebuffer (the window)
```

Two threads exist: the **engine thread** runs `Engine::update()` (game logic,
ECS mutation) at a fixed 240 Hz tick, while the **render thread** (owns the GL
context) runs `GLFWRenderingSystem::start()`'s loop, which calls `render()` once
per `glfwSwapBuffers`. They hand data across via mutex-guarded "dirty" snapshots
(§3.3) and `std::promise`/`std::future` job queues for asset loading (§3.4) — the
render thread is the only thread allowed to touch GL objects.

---

## 2. Mathematical foundations

This section builds up every mathematical tool the pipeline relies on, then shows
exactly where it appears in code.

### 2.1 Points, vectors, and homogeneous coordinates

A 3D point or direction is a column vector. To represent **translation** as a
matrix multiplication (matrices alone can only do linear maps: rotation, scale,
shear — never translation, since a linear map always sends the origin to the
origin), graphics uses **homogeneous coordinates**: a 3-vector `(x, y, z)` is
extended to a 4-vector `(x, y, z, w)`.

- `w = 1` → a **point** (affected by translation)
- `w = 0` → a **direction/vector** (not affected by translation — translating a
  direction makes no sense)

A 4×4 matrix can then encode rotation + scale + translation in one object:

```
⎡ R₀₀ R₀₁ R₀₂ tx ⎤   ⎡x⎤   ⎡ x' ⎤
⎢ R₁₀ R₁₁ R₁₂ ty ⎥ × ⎢y⎥ = ⎢ y' ⎥
⎢ R₂₀ R₂₁ R₂₂ tz ⎥   ⎢z⎥   ⎢ z' ⎥
⎣  0   0   0  1  ⎦   ⎣1⎦   ⎣ 1  ⎦
```

The 3×3 upper-left block `R` does rotation/scale, the last column `t` does
translation. This whole family of "rotate+scale+translate, last row `0 0 0 1`"
matrices is called the **affine transformations**, and Eigen represents them with
`Eigen::Affine3f` / `Eigen::Projective3f` (the latter allows a non-trivial last
row, needed for perspective). `citty::engine::Transform`
(`citty/engine/components/Transform.hpp:12`) stores rotation/position/scale
*decomposed* (quaternion + vector + vector) rather than as a matrix — the matrix
is only assembled when needed (§3.3), because decomposed form is cheaper to
interpolate/animate and trivially composable hierarchically.

### 2.2 Composing transforms: object → world

A model matrix `M` is built as `T · R · S` (translate, then rotate, then scale —
read right to left since these act on column vectors): scale the local mesh first,
then orient it, then place it in the world. In code
(`citty/graphics/GLFWRenderingSystem.cpp:240-243`):

```cpp
transformMatrix = Eigen::Translation3f(transform.position)
                 * transform.rotation
                 * Eigen::AlignedScaling3f{transform.scale};
```

This is exactly `T · R · S`. For a vertex `v` in object space, the world-space
position is `M · v = T · R · S · v`: `v` is scaled first (innermost), then
rotated, then translated — matrix multiplication order **reads right-to-left**
as "the operation closest to the vector happens first".

**Parented (hierarchical) transforms.** If `Transform::parent` is set
(`citty/engine/components/Transform.hpp:16`), the world matrix is the chain
product of every ancestor's local matrix, root-to-leaf:

```
M_world(child) = M_world(parent) · M_local(child)
```

`GLFWRenderingSystem::handleGraphicsEntities` (lines 213–243) walks up the
parent chain pushing each `T·R·S` onto a stack (so the walk order is
leaf→root), then pops the stack (root→leaf) multiplying left-to-right into the
final `transformMatrix`. This guarantees a child inherits the parent's full
transform stack — e.g. a wheel entity parented to a car entity rotates and
translates with the car, then applies its own local spin on top.

### 2.3 Matrix storage convention: column-major, column vectors

OpenGL's historical (and `citty`'s) convention is: vectors are columns, and
matrices are applied on the left (`v' = M·v`), and the 16 floats of a `mat4`
are stored **column by column**. Eigen's default storage is *also*
column-major, and `ShaderProgram::setUniform(..., Eigen::Matrix4f value, bool
transpose=false)` (`citty/graphics/ShaderProgram.cpp:55`) uploads the raw
matrix memory untransposed:

```cpp
glProgramUniformMatrix4fv(programName, location, 1, transpose, &value(0));
```

So `Eigen::Affine3f::matrix()` (row `i`, column `j`, mathematical notation) ends
up addressed in GLSL as `mat[j][i]` (GLSL indexes `mat4` as `mat[column]`,
each column itself a `vec4`). This is *exactly* why
`shaders/lightCulling.csh:62` reads `projection[3][2]` and `projection[2][2]` to
get the mathematical entries `P(2,3)` and `P(2,2)` that
`Math.cpp:18-21` wrote with Eigen's `(row, col)` operator — same matrix, two
different indexing conventions for the same column-major memory. Getting this
backwards is one of the most common bugs in any home-grown renderer, so it's
worth internalizing: **GLSL `m[c][r]` ≡ math/Eigen `M(r, c)`.**

### 2.4 Coordinate spaces and the pipeline of transforms

A vertex passes through a strict chain of coordinate spaces before becoming a
pixel:

```
object/local space --M(model)--> world space --V(view)--> view/eye space
        --P(projection)--> clip space --(÷w)--> NDC --viewport--> screen space
```

- **Object space**: raw mesh data as loaded (`Vertex::position`).
- **World space**: after the model matrix; all entities share this space, which
  is where lighting in this engine is conceptually anchored before being moved
  into other spaces.
- **View/eye space**: world space re-expressed relative to the camera, with the
  camera at the origin looking down a fixed axis. Built by the *view matrix*.
- **Clip space**: after the projection matrix; a 4D homogeneous space where the
  viewing frustum has been sheared/scaled into a canonical shape, but the
  perspective divide hasn't happened yet (this is required because the GPU
  needs `w` to clip primitives against the frustum *before* dividing, otherwise
  triangles behind the camera would invert).
- **NDC (Normalized Device Coordinates)**: `clip.xyz / clip.w`, a cube
  `[-1,1]³` (and `z ∈ [-1,1]` maps to depth `[0,1]` under the default
  `glDepthRange`). The GPU performs this divide automatically after the vertex
  shader.
- **Screen space**: NDC mapped into the viewport's pixel rectangle by
  `glViewport`.

Every vertex shader in this codebase performs object→clip directly in one line,
e.g. `shaders/lightAccumulation.vsh:26`:

```glsl
gl_Position = projection * view * model * vec4(position, 1.0);
```

reading right-to-left: `position` (object space, `w=1` appended) is first
placed in the world by `model`, then re-expressed relative to the camera by
`view`, then projected into clip space by `projection`. The GPU then performs
`gl_Position.xyz / gl_Position.w` automatically (perspective divide) and maps
the result through the viewport transform to get a screen pixel.

**This engine's non-standard world axis convention.** Most OpenGL tutorials
assume "world forward = −Z". `citty` instead treats **+X as forward** and
**+Y as up** in world space — see `citty.cpp:46-53`: cars are placed by
varying world `Z` so that, looking down `+X`, they fan out left-to-right across
the screen, and the camera is aimed with
`renderingEngine->setViewpoint(position, rotation)` →
(`GLFWRenderingSystem.cpp:37`) `lookAt(position, position + rotation *
Eigen::Vector3f::UnitX(), rotation * Eigen::Vector3f::UnitY())`
(`citty/graphics/Math.cpp:25`). The `lookAt` function (§2.6) is exactly the
adapter that converts this arbitrary world-forward axis into OpenGL's
expected view-space convention (camera at the origin, looking down view `−Z`),
so everything downstream of the view matrix (projection, depth linearization,
frustum planes) can use the standard formulas without caring what "forward"
meant in world space.

### 2.5 Quaternions for rotation

`Transform::rotation` is an `Eigen::Quaternionf` (`citty/engine/components/Transform.hpp:13`).
A unit quaternion `q = w + xi + yj + zk` (`‖q‖ = 1`) represents a rotation by
angle `θ` around a unit axis `**a**` as:

```
q = (cos(θ/2), sin(θ/2)·a)
```

Rotating a vector `**v**` is `v' = q v q⁻¹` (using quaternion multiplication
with `v` embedded as a pure quaternion `(0, v)`). Quaternions are preferred over
Euler angles or raw matrices for animated rotation because:
- they compose by simple multiplication: `q_total = q₂ · q₁` applies `q₁`
  then `q₂` (`citty.cpp:92`: `carEntity...rotation *= rotationDelta;`
  incrementally spins entities frame to frame);
- they don't suffer gimbal lock;
- they're trivially renormalized to fight floating-point drift (`‖q‖→1`).

When a world matrix is assembled (§2.2), `Eigen::Quaternionf` implicitly
converts to its equivalent 3×3 rotation matrix via the standard formula

```
R = ⎡ 1−2(y²+z²)   2(xy−wz)     2(xz+wy)   ⎤
    ⎢ 2(xy+wz)     1−2(x²+z²)   2(yz−wx)   ⎥
    ⎣ 2(xz−wy)     2(yz+wx)     1−2(x²+y²) ⎦
```

### 2.6 Building the view matrix: `lookAt`

`citty/graphics/Math.cpp:25-37` builds an orthonormal basis (`right`, `up`,
`front`) from a camera position, a target, and a world-up hint, via Gram-Schmidt:

```cpp
front = normalize(cameraPos - targetPosition);  // points *backward*, away from view dir
right = cross(up, front);                       // (up assumed already ⟂ front-ish)
```

then assembles the view matrix as:

```
        ⎡ rightx   righty   rightz   −right·cameraPos ⎤
View =  ⎢ upx      upy      upz      −up·cameraPos    ⎥
        ⎢ frontx   fronty   frontz   −front·cameraPos ⎥
        ⎣ 0        0        0         1                ⎦
```

This is the classic "rotation-then-translation, but expressed as one matrix"
trick: the view matrix is the *inverse* of the camera's own world transform.
For a pure-rotation matrix, the inverse equals the transpose, so the rows of
`View`'s rotation part are simply the camera's basis vectors written as rows
(instead of columns, as they'd appear in the camera's own world matrix). The
translation column `(−right·cameraPos, −up·cameraPos, −front·cameraPos)` is
`−Rᵀ·cameraPos`, which correctly carries the camera position through the
inverted rotation. The effect: any world point gets re-expressed with the
camera at the origin, `right` along view `+X`, `up` along view `+Y`, and
`front` (= "backward", since it points from target to camera) along view `+Z` —
i.e. the camera looks down view **−Z**, OpenGL's standard convention,
regardless of what axis was "forward" in world space (§2.4).

### 2.7 Perspective projection, derived

The projection matrix maps the camera's view frustum (a truncated pyramid
defined by a vertical field of view `fovY`, an `aspectRatio`, and near/far
planes `zNear`,`zFar`) into the canonical clip-space cube.

**Deriving the X/Y scale.** Consider a point at view-space depth `z` (negative,
since the camera looks down −Z) with vertical extent `y`. The half-height of
the visible frustum slice at that depth is `|z|·tan(fovY/2)` (basic right-
triangle trig: the frustum opens at angle `fovY/2` from the view axis). To map
that visible extent to NDC `[-1, 1]`, divide by it:

```
NDC_y = y / (|z| · tan(fovY/2))
```

After the perspective divide (`NDC = clip.xyz / clip.w`), and choosing
`clip.w = −z` (see below), the vertex shader's job is just to produce
`clip.y = y / tan(fovY/2)` so that dividing by `clip.w = −z` reproduces the
formula above. Hence the matrix entry `P(1,1) = 1/tan(fovY/2) = cot(fovY/2)`.
The X axis gets an extra `1/aspectRatio` factor to account for non-square
viewports: `P(0,0) = 1/(aspectRatio·tan(fovY/2))`.

This is exactly `perspectiveProjection()` in `citty/graphics/Math.cpp:8-23`:

```cpp
float tanHalfFoV = std::tan(verticalFoV / 2.0f);
projection(0, 0) /= aspectRatio * tanHalfFoV;   // = 1 / (aspect·tan(fov/2))
projection(1, 1) /= tanHalfFoV;                 // = 1 / tan(fov/2)
```

**Deriving the Z mapping.** Z needs a *non-linear* map: it must send
`z = −zNear → NDC_z = −1` and `z = −zFar → NDC_z = +1`, while preserving
ordering (so the depth buffer remains useful for depth testing), using only an
affine function of `z` divided by `w = −z` (so that the divide hardware does
the nonlinear part). Set `clip.z = A·z + B`, `clip.w = −z`. Solving
`(A·(−zNear)+B)/zNear = −1` and `(A·(−zFar)+B)/zFar = 1` simultaneously gives:

```
A = −(zFar+zNear)/(zFar−zNear)        B = −2·zFar·zNear/(zFar−zNear)
```

which is exactly:

```cpp
projection(2, 2) = -(zFar + zNear) / (zFar - zNear);              // A
projection(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);       // B
projection(3, 2) = -1.0f;                                          // makes clip.w = -z
projection(3, 3) = 0.0f;
```

The hyperbolic (1/z-ish) nature of this map is precisely why depth precision is
heavily weighted toward the near plane, and why the light-culling pass has to
explicitly *undo* this nonlinearity before it can compute a per-tile min/max
depth in linear view-space units (§6.4, §2.11).

Field of view, near plane, and far plane are fixed engine-wide constants
(`kFieldOfView = 45°`, `kNearPlane = 0.1`, `kFarPlane = 100.0`,
`GLFWRenderingSystem.cpp:18-20`), recomputed whenever the window resizes
(`onWindowSizeChange`, line 462) since `aspectRatio` depends on the
framebuffer's width/height.

### 2.8 Transforming normals: the inverse-transpose rule

A surface normal is **not** a position — it's a direction perpendicular to a
surface, and under a non-uniform scale or shear, naively multiplying a normal
by the same matrix `M` used for positions can make it stop being perpendicular
to the (correctly) transformed surface. The correct transform for normals is
the **inverse transpose** of the upper-left 3×3 of the model matrix:

```
N' = (M⁻¹)ᵀ · N
```

*Why:* a surface can be locally described by a tangent plane equation
`N·v = 0` (`N`, `v` in object space). Substituting `v = M·v'` (`v'` the
world-space position) requires `N·M·v' = 0`, i.e. `(Mᵀ N)·v' = 0` — that's the
wrong direction (`Mᵀ`, not what we want). Defining `N' = (M⁻¹)ᵀ N` instead and
checking `N'·v' = (M⁻¹)ᵀN · v' = Nᵀ M⁻¹ v'`... the clean way to see it: we want
`N'` such that `N'·t = 0` for every tangent `t` of the *transformed* surface,
where `t = M·t'` for an object-space tangent `t'` satisfying `N·t' = 0`. Take
`N' = (M⁻¹)ᵀN`: then `N'·t = (M⁻¹)ᵀN · Mt' = Nᵀ M⁻¹ M t' = Nᵀ t' = N·t' = 0`. ✓.
For pure rotations `M⁻¹ = Mᵀ`, so `(M⁻¹)ᵀ = M`— normals transform the same as
positions and this subtlety vanishes; it only bites with non-uniform scale.

This is exactly `shaders/lightAccumulation.vsh:30`:

```glsl
mat3 normalMatrix = transpose(inverse(mat3(model)));
```

applied to the normal, tangent, and bitangent alike.

### 2.9 Tangent space and the TBN matrix

Normal maps store perturbed normals in **tangent space** — a per-vertex local
coordinate frame where `+Z` follows the surface normal, and `+X`/`+Y` follow
the direction texture-coordinates `u`/`v` increase across the surface (so the
normal map can be authored independent of the mesh's orientation in world
space, and tiled/reused across different surfaces). The basis vectors are:

- **T** (tangent) — direction of increasing `u`, supplied per-vertex by Assimp's
  `aiProcess_CalcTangentSpace` (`GLFWRenderingSystem.cpp:259`) and stored in
  `Vertex::tangent`.
- **B** (bitangent) — direction of increasing `v`, `Vertex::bitangent`.
- **N** (normal) — `Vertex::normal`.

`[T B N]`, after the inverse-transpose normal-matrix transform and
normalization, form the matrix that maps tangent-space vectors → world space.
Its **transpose** (since `T,B,N` are orthonormal, the inverse of an orthonormal
matrix is its transpose) maps world space → tangent space — this is the `TBN`
built in `shaders/lightAccumulation.vsh:36`:

```glsl
mat3 TBN = transpose(mat3(tan, bitan, norm));
```

The engine chooses to do all lighting math **in tangent space** rather than
transform the sampled normal-map normal into world space — this is a common
optimization, since it means only a handful of per-fragment vectors (view
position, fragment position, light position) need the `TBN` multiply, instead
of multiplying every light against a world-space normal computed by inverting
the TBN per fragment. `tangentViewPosition` and `tangentFragmentPosition` are
precomputed per-vertex (interpolated by the rasterizer) at
`lightAccumulation.vsh:37-38`, and each light's position is transformed
per-fragment at `lightAccumulation.fsh:77`.

### 2.10 The reflectance model: Lambertian diffuse + Blinn-Phong specular

Local (direct) lighting at a surface point is modeled as the sum of a
**diffuse** term (light scattered equally in all directions — matte surfaces)
and a **specular** term (light reflected in a narrow cone around the mirror
direction — glossy highlights).

**Diffuse (Lambert's cosine law).** A surface receives less light per unit
area as the light direction grows oblique to the surface normal — the same
reason sun intensity varies with latitude/season. If `**L**` is the unit
vector from the surface toward the light and `**N**` the unit surface normal:

```
diffuse = max(N · L, 0)
```

clamped at zero because a negative dot product means the light is *behind*
the surface (self-shadowed). Code: `lightAccumulation.fsh:89`.

**Specular (Blinn-Phong).** The classic Phong model compares the *reflection*
of `L` about `N` to the view direction `V`; Blinn's modification (cheaper, and
generally a closer match to real materials) instead computes the **halfway
vector** between `L` and `V`:

```
H = normalize(L + V)
specular = max(N · H, 0) ^ shininess
```

`H` points "as if" the surface were tilted to mirror-reflect the light
straight into the eye; the closer `N` is to `H`, the stronger the highlight.
Raising to the power `shininess` controls how tight the highlight is — higher
exponent ⇒ smaller, sharper specular hot-spot (a proxy for micro-facet
roughness, without doing a full microfacet BRDF). Code:
`lightAccumulation.fsh:86,90`.

**Combining them**, with per-light color `Lc`, surface albedo `Kd`
(`base_diffuse`) and specular tint `Ks` (`base_specular`), and attenuation
`A(d)` (§2.10b):

```
irradiance = Lc · (Kd·diffuse + Ks·specular) · A(d)
color += irradiance
```

(`lightAccumulation.fsh:97-98`), summed over every light visible in the
fragment's tile, plus a flat **ambient** term `0.08·Kd` added once
(`lightAccumulation.fsh:101`) so unlit surfaces aren't pure black — a crude
stand-in for global illumination / indirect bounce light, which this engine
doesn't otherwise simulate.

One implementation detail worth flagging:
`lightAccumulation.fsh:93-95` zeroes `specular` whenever `diffuse == 0`. This
is a manual patch for a well-known Blinn-Phong artifact: `N·H` can stay
positive (and thus produce a visible specular highlight) even when `N·L ≤ 0`
(light behind the surface), because `H` is a blend of `L` and `V`, not `L`
alone — without the guard, objects can show a glowing specular rim on their
unlit side as a light passes behind them.

### 2.10b Distance attenuation

Physically, point-light irradiance falls off with the **inverse square law**,
`I(d) = I₀ / d²`, because the same total light energy spreads over a sphere of
surface area `4πd²` as distance `d` grows. A pure inverse-square function never
reaches exactly zero, though — which is a problem for a *tiled* renderer,
because the light-culling pass (§2.11) needs a **hard radius** to cull lights
against per-tile frustums; if shading used a different, unbounded falloff, a
light could be culled (treated as zero contribution) while the shading
function still expected to contribute a small but nonzero amount, causing
visible "popping" at the boundary.

`attenuate()` (`lightAccumulation.fsh:38-45`) instead computes a windowed
falloff that is smooth, inverse-square-*like* near the light, and reaches
*exactly* zero at a distance tied to `radius`:

```glsl
float attenuate(vec3 lightDirection, float radius) {
    float cutoff = 0.5;
    float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius); // d² / (100·r)
    attenuation = 1.0 / (attenuation * 15.0 + 1.0);                             // 1 / (15·d²/(100r) + 1)
    attenuation = (attenuation - cutoff) / (1.0 - cutoff);                      // remap [0.5,1]→[0,1]
    return clamp(attenuation, 0.0, 1.0);
}
```

Step by step: `dot(lightDirection, lightDirection) = d²` (the function is
deliberately called *before* `lightDirection` is normalized —
`lightAccumulation.fsh:81-82` — precisely so this squared-distance shortcut
works without a redundant `sqrt`/re-square). The middle line is a smoothed
reciprocal falloff `1/(k·d²+1)` (avoids the singularity of `1/d²` at `d=0`,
unlike the true inverse-square law). The final remap **linearly stretches**
the range `[cutoff, 1] → [0, 1]` and clamps — so any raw attenuation value
`≤ 0.5` (i.e. far enough from the light) is driven to exactly `0`. The
`100·radius`/`15` constants are tuned so the zero-crossing roughly lines up
with the `radius` value used by the culling pass's sphere test, keeping
shading and culling visually consistent.

### 2.11 Tile frustum construction and the light-sphere test

The whole point of light culling is a geometric question, asked once per
screen tile per light: **does this point light's bounding sphere intersect
this tile's view-frustum slice?** This needs two ingredients: (a) the six
planes bounding the tile's frustum, and (b) a sphere-plane intersection test.

**A plane in 3D** is the set of points `**v**` satisfying `**n**·**v** + d = 0`
for unit normal `**n**` and scalar offset `d`. Equivalently, stored as a 4-vector
`(nx, ny, nz, d)` so that `plane·(v,1) = n·v + d`. The **signed distance** from
an arbitrary point `**p**` to the plane is exactly `plane·(p,1)`: positive on the
side the normal points toward, negative on the other side, zero on the plane.

**Tile side planes.** Recall NDC space is the cube `[-1,1]³`
(§2.4). A tile occupies a rectangular sub-region of the screen, which
corresponds to a rectangular sub-region of NDC `x,y ∈ [-1,1]`. The four side
planes of a tile's frustum, in clip/NDC space, are therefore *trivial* —
simple axis-aligned planes through whatever NDC x/y bounds the tile covers.
`lightCulling.csh:78-87` computes those bounds from the tile's grid position:

```glsl
vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);          // tile's NDC left/bottom edge, mapped to [0,2]
vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1,1))) / vec2(tileNumber); // tile's NDC right/top edge

frustumPlanes[0] = vec4( 1, 0, 0,  1 - negativeStep.x);  // Left   plane: x ≥ tileLeft   (in NDC)
frustumPlanes[1] = vec4(-1, 0, 0, -1 + positiveStep.x);  // Right  plane: x ≤ tileRight
frustumPlanes[2] = vec4( 0, 1, 0,  1 - negativeStep.y);  // Bottom plane
frustumPlanes[3] = vec4( 0,-1, 0, -1 + positiveStep.y);  // Top    plane
```

These four planes are defined directly in clip space, so they must be
transformed back into **world space** (the space the light positions live in)
before they're useful. A clip-space plane equation `p·v_clip = 0` with
`v_clip = (view·proj)·v_world` becomes `p·(VP·v_world) = 0 = (pᵀVP)·v_world`,
i.e. the world-space plane is `p·VP` treating `p` as a *row* vector — exactly
`frustumPlanes[i] *= viewProjection` at `lightCulling.csh:91` (GLSL's
`vec * mat` is row-vector-times-matrix, i.e. `transpose(mat)*vec` in
column-vector terms). This is the standard trick: planes transform with the
forward matrix when they're already expressed in the *target* (clip) space of
that matrix, unlike points/normals which need the matrix or its
inverse-transpose respectively — no matrix inversion required here. The result
is then renormalized (`/= length(xyz)`) since the transform can change the
normal's magnitude.

**Near/far planes & depth linearization.** The near/far planes need the
tile's `[minDepth, maxDepth]`, read from the depth-prepass texture
(`lightCulling.csh:60`) and reduced across the tile's 256 threads with
`atomicMin`/`atomicMax` on the bit pattern of the float (`floatBitsToUint`,
lines 65-67 — a standard trick: for non-negative floats, comparing the raw
IEEE-754 bit pattern as an unsigned integer gives the same ordering as
comparing the floats, which is what makes `atomicMin`/`atomicMax`, integer-only
GLSL built-ins, usable on depth values). But the depth buffer holds
*post-projection, post-divide* depth (§2.7), which is **not linear** in
view-space distance — averaging or min/max-ing it directly would be wrong, so
it's first linearized back to view-space `z`:

```glsl
depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);
```

Deriving this: from §2.7, `clip.z = A·z + B`, `clip.w = −z`
(`A = projection(2,2)`, `B = projection(2,3)`, addressed in GLSL as
`projection[2][2]`/`projection[3][2]` per the column-major convention of
§2.3). The depth-buffer value (after the `[-1,1]→[0,1]` remap the GPU performs)
is `depthBuf = 0.5·(clip.z/clip.w) + 0.5 = 0.5·(Az+B)/(−z) + 0.5`. Solving for
`z`:

```
depthBuf − 0.5 = −0.5(Az+B)/z
z·(depthBuf − 0.5) = −0.5Az − 0.5B
z·(depthBuf − 0.5 + 0.5A) = −0.5B
z = −0.5B / (depthBuf + 0.5A − 0.5)
```

Since both `A` and `B` are negative for `zFar > zNear > 0` (§2.7), `−0.5B` is
positive, and the shader's `0.5·projection[3][2] = 0.5B` (without the
negation) divided by the *same* denominator yields `−z`, i.e. the *positive*
distance in front of the camera — which is exactly the convention the rest of
the pass wants (`minDepth`/`maxDepth` as positive distances,
`frustumPlanes[4] = vec4(0,0,-1,-minDepth)`, `frustumPlanes[5] =
vec4(0,0,1,maxDepth)`, i.e. planes whose normals point along view `±Z`,
offset by those distances, transformed into world space the same way as the
side planes but using `view` alone (no projection — they're already expressed
along the view-space Z axis, not clip space), at `lightCulling.csh:96-99`.

**Sphere-frustum intersection.** With six world-space planes in hand, testing
a light of world position `P` and radius `r` against the frustum is a loop of
six signed-distance tests (`lightCulling.csh:121-128`):

```glsl
for (j = 0; j < 6; j++) {
    distance = dot(position, frustumPlanes[j]) + radius;
    if (distance <= 0.0) break;   // sphere entirely outside this plane ⇒ no intersection
}
```

`dot(position, frustumPlanes[j])` is the signed distance from the light center
to plane `j` (treating `frustumPlanes[j]` as `(n, d)` and `position` as
`(p, 1)`, so the dot product already includes the `+d` offset term — see the
plane definition above). If that signed distance is more negative than
`−radius`, the entire sphere is on the wrong side of that one plane, and by
separating-axis logic the sphere cannot intersect the frustum (it's a
necessary, if not perfectly tight, conservative test — spheres can have false
positives near frustum edges, which is an accepted trade-off for the
cheapness of 6 dot products per light per tile vs. exact frustum-sphere
geometry).

### 2.12 HDR and tone mapping

Real-world luminance has enormous dynamic range (a lit point light close-up
can be far "brighter" than `1.0`); accumulating lighting in the engine's
floating-point HDR color buffer (`SizedImageFormat::RGB8`... actually see
note in §4.5 about the half-float vs RGB8 nuance) lets per-light contributions
sum past `1.0` without clamping prematurely, deferring the "how do I squeeze
this into a displayable `[0,1]` range" question to one final, deliberate
step: **tone mapping**.

`hdr.fsh` implements the **Reinhard operator** with an exposure control:

```
result = 1 − e^(−color · exposure)
```

(`hdr.fsh:18`). As `color·exposure → ∞`, `result → 1` asymptotically (never
clips abruptly — preserves relative differences between very bright values,
unlike naive `clamp(color, 0, 1)`); as `color·exposure → 0`, `result ≈
color·exposure` (linear near black, by the first-order Taylor expansion of
`eˣ`). `exposure` acts as a pre-multiplier — raising it brightens dark areas
faster than it compresses bright ones, mimicking a camera's exposure dial.

**Gamma correction.** Display devices and human brightness perception are
both roughly logarithmic/power-law, not linear — a `sRGB` framebuffer expects
values pre-encoded with `output = linear^(1/2.2)` so that after the display's
own `^2.2`-ish decoding, perceived brightness steps look uniform. Without this
step, lighting that's mathematically correct in linear space looks washed out
and too dark in the midtones once displayed. `hdr.fsh:21-22`:

```glsl
const float gamma = 2.2;
result = pow(result, vec3(1.0 / gamma));
```

---

## 3. ECS architecture and how it feeds the renderer

### 3.1 Archetype-based storage

`citty::engine` is a from-scratch **archetype ECS**, in the same family as
Unity DOTS / EnTT's grouped storage. The core idea: entities with an identical
*set* of component types (their **archetype**) are stored together in
column-oriented (struct-of-arrays) tables, so iterating "all entities with
`Transform` and `Graphics`" touches tightly packed, cache-friendly memory
instead of chasing pointers through heterogeneous per-entity objects.

- **`Archetype`** (`citty/engine/Archetype.hpp:21`) = `std::set<std::type_index>`,
  wrapped in a `boost::flyweight` (`ArchetypeFlyweight`) so that identical
  archetype sets are interned/deduplicated and can be used as cheap hash-map
  keys (comparisons become pointer/id comparisons under the hood).
- **`ArchetypeRecord`** (`citty/engine/ArchetypeRecord.hpp`) is the table for
  one archetype: a `std::vector<EntityId>` of member entities, plus one
  type-erased `ComponentContainer` (effectively a `std::vector<T>`) per
  component type in the archetype, kept in lockstep index order — i.e.
  `entities[i]`'s `Transform` lives at index `i` of the `Transform` container,
  same `i` for `Graphics`, etc.
- **`ArchetypeGraph`** (`citty/engine/ArchetypeGraph.hpp`) memoizes the edges
  between archetypes: "archetype X + component `T` → archetype Y". Adding a
  component to an entity (`ComponentStore::add`, `ComponentStore.hpp:31-52`)
  looks up (or builds, on first use) the destination archetype and **moves**
  every existing component value for that entity into the new archetype's
  containers (`ArchetypeRecord::moveToNextArchetype`,
  `ArchetypeRecord.hpp:61-73`), then constructs the new component in place at
  the end. This is the classic ECS trade-off: O(entity's component count)
  data movement on structural changes (rare), in exchange for O(1)-stride,
  branch-free iteration during queries (every frame, for every system).
- **Queries** (`ComponentStore::getAll<Ts...>()`, `ComponentStore.hpp:127-164`)
  find every archetype that is a **superset** of the requested component set
  (`ArchetypeGraph::getSupersets`, `ArchetypeGraph.cpp:10-33` — entities with
  *extra* components beyond what's requested still match), then `join` each
  matching archetype's column for each requested type into one flattened
  range, and `zip_view`s those per-type ranges together so a caller iterating
  the result gets `(Transform&, Graphics&)` tuples lined up correctly across
  archetype boundaries.

### 3.2 The components that matter for rendering

| Component | File | Fields | Role |
|---|---|---|---|
| `engine::Transform` | `citty/engine/components/Transform.hpp` | `rotation` (quaternion), `position`, `scale`, optional `parent` | Where an entity is, hierarchically |
| `graphics::Graphics` | `citty/graphics/components/Graphics.hpp` | `mesh` id, `material` id | "this entity is a drawable instance of mesh+material" |
| `graphics::PointLight` | `citty/graphics/components/PointLight.hpp` | `color`, `radius` | Light source; position comes from the entity's `Transform` |
| `graphics::Camera` | `citty/graphics/components/Camera.hpp` | `fov` | Present but not yet wired into the active camera path (the live camera is driven directly via `RenderingEngine::setViewpoint`/`setProjection` from `citty.cpp`, not by iterating `Camera` components) |

Note the deliberate **decoupling**: nothing about `Graphics` or `PointLight`
stores a position — they piggyback on whatever `Transform` is attached to the
same entity, queried jointly (`getComponents<engine::Transform, Graphics>()`,
`GLFWRenderingSystem.cpp:205`). This is idiomatic ECS composition: "drawable"
and "located in space" are orthogonal concerns, combined only by both being
present on the same entity ID.

### 3.3 `GLFWRenderingSystem`: the ECS → renderer bridge

`GLFWRenderingSystem` (`citty/graphics/GLFWRenderingSystem.hpp/.cpp`) is an
`engine::System` (so `Engine::update()` calls its `update()` once per engine
tick, `Engine.cpp:19-23`) that also owns the `RenderingEngine` and the GLFW
window/render loop.

Each engine tick, `update()` (line 63) calls:

- **`handleGraphicsEntities()`** (line 204): queries `(Transform, Graphics)`,
  resolves each entity's full world matrix (walking the parent chain as
  described in §2.2), and packages the result as a flat
  `std::vector<GraphicsEntity>` (`citty/graphics/GraphicsEntity.hpp:10` —
  `{Eigen::Affine3f transform; meshId; materialId;}`) — a render-ready,
  ECS-agnostic snapshot. This is written under `graphicEntityMutex` and the
  `graphicEntitiesDirty` flag is set (line 249).
- **`handlePointLightEntities()`** (line 446): queries `(Transform,
  PointLight)`, and packages each into `PointLightEntity`
  (`citty/graphics/PointLightEntity.hpp` — `{Vector4f position; Vector4f
  color; float radius;}`, position/color padded to `vec4` to match the GLSL
  `std430` layout, §5.5), written under `pointLightMutex`.

This split exists because the **engine thread** (running game logic at 240 Hz,
`citty.cpp:81-98`) and the **render thread** (running at display refresh rate,
calling `GLFWRenderingSystem::render()`, line 68) are different threads. The
render thread's `uploadGraphicsEntities()`/`uploadPointLightEntities()`
(lines 77, 90) atomically swap out the dirty snapshot under the same mutex and
hand it to `RenderingEngine::setGraphicsEntities`/`setPointLightEntities`
*only if it actually changed* since the last frame (`graphicEntitiesDirty`
check) — so a render thread running faster than the engine tick simply
re-renders the last snapshot instead of blocking.

### 3.4 Asset loading: Assimp import + async queues

`loadModel()` (`GLFWRenderingSystem.cpp:252`) uses **Assimp** to import an
arbitrary model file (the demo loads `assets/car.obj`,
`citty.cpp:44`), with post-process flags `aiProcess_Triangulate` (ensure all
faces are triangles — what the renderer's `DrawMode::TRIANGLES` expects),
`aiProcess_GenSmoothNormals` (compute vertex normals if the source file lacks
them), `aiProcess_JoinIdenticalVertices` (vertex dedup, enabling indexed
rendering), `aiProcess_FixInfacingNormals`, and
`aiProcess_CalcTangentSpace` (computes the tangent/bitangent vectors §2.9
needs for normal mapping). Each Assimp mesh/material becomes a `citty::Mesh`/
`citty::Material` via `loadAssimpMesh`/`loadAssimpMaterial`
(lines 374-444), and the node hierarchy becomes a `Model` tree
(`citty/graphics/Model.hpp`) that `buildModelInstance()` (line 325) later
instantiates into real ECS entities (one entity per node, parented per the
source hierarchy, each leaf mesh getting its own child entity with a
`Graphics` component — lines 333-353).

Because only the render thread may safely call GL functions, but
`loadMesh`/`loadMaterial`/`loadTexture` can be invoked from *any* thread
(e.g. the engine thread, mid-tick), they don't touch the `RenderingEngine`
directly. Instead they push a `(std::promise<id>, payload)` tuple onto a
mutex-guarded queue (`textureLoadQueue`/`materialLoadQueue`/`meshLoadQueue`,
`GLFWRenderingSystem.hpp:114-119`) and block on the paired
`std::future::get()` (e.g. `loadMesh`, lines 194-202). The render thread
drains these queues once per frame, *before* rendering
(`processLoadingQueues()`, called from `render()` at line 69), actually
calling into `RenderingEngine::loadMesh/loadMaterial/loadTexture` (which
issue the real GL calls) and fulfilling the promise — unblocking whichever
thread requested the load. `loadTexture` additionally de-duplicates by
canonicalized filesystem path (`loadedTextures` map,
`GLFWRenderingSystem.hpp:110-112`) so the same texture file is never uploaded
twice.

---

## 4. OpenGL object setup

### 4.1 Context & global state

The context is created via GLFW (`citty.cpp:23`, `glfwCreateWindow`) and made
current on the render thread (`GLFWRenderingSystem.cpp:30`). Function loading
is handled by **libepoxy** (`#include <epoxy/gl.h>` / `gl_generated.h`
throughout), which dynamically resolves GL entry points at call time — no
explicit `gladLoadGL()`-style init call is needed in this codebase because
epoxy resolves lazily per-call.

`RenderingEngine`'s constructor (`RenderingEngine.cpp:14-18`) sets the global
state that holds for the whole app lifetime:

```cpp
glEnable(GL_DEPTH_TEST);   // enable the depth buffer for hidden-surface removal
glDepthMask(GL_TRUE);      // depth writes enabled (the depth prepass needs this)
glEnable(GL_CULL_FACE);    // back-face culling (default: cull CCW-wound back faces)
glEnable(GL_MULTISAMPLE);  // enable MSAA resolve if the default framebuffer is multisampled
```

`glClearColor(0.4, 0.4, 0.8, 1)` (line 62) sets a light-blue clear color, used
only by the (unused, since color writes are disabled in the depth pass and the
HDR pass fully overwrites every pixel via a fullscreen quad) default
framebuffer clear.

### 4.2 Direct State Access (DSA)

Every wrapper class (`Buffer<T>`, `VertexArray`, `Texture`, `Framebuffer`,
`Renderbuffer`) is written exclusively against **OpenGL 4.5's DSA API**
(`glCreate*` + `gl*Named*`/`glVertexArrayAttrib*` functions) rather than the
older bind-to-edit style (`glGenBuffers`+`glBindBuffer`+`glBufferData`).
For example, `Buffer<T>`'s constructor uses `glCreateBuffers` (allocates a
named object immediately, no separate "first bind initializes the type" step)
and `glNamedBufferData`/`glNamedBufferSubData` (`Buffer.hpp:35-39,70,108,119`)
operate directly on an object name without binding it to any target first.
This both simplifies the wrapper code (no implicit "currently bound object"
state machine to track across calls) and avoids a class of bugs where an
unrelated bind call elsewhere invalidates an assumption another piece of code
made about what's currently bound. `VertexArray::bindBuffer` similarly uses
`glVertexArrayVertexBuffer`/`glVertexArrayAttribFormat`/
`glVertexArrayAttribBinding` (`VertexArray.hpp:48,69-70`) to configure a VAO's
attribute layout without ever calling `glBindVertexArray` except at actual
draw time (`VertexArray::bind()`, `VertexArray.cpp:61-64`, only invoked from
inside `draw()`/`drawElementsInstanced()`).

### 4.3 Shader compilation & program linking

Three small classes form the shader pipeline:

- **`Shader`** (`citty/graphics/Shader.cpp`) reads a GLSL source file from
  disk, calls `glCreateShader`+`glShaderSource`+`glCompileShader`, and checks
  `GL_COMPILE_STATUS`, throwing on failure.
- **`ShaderProgramBuilder`** (`citty/graphics/ShaderProgramBuilder.cpp`)
  collects one or more `Shader`s (e.g. a `.vsh` + `.fsh` pair, or a lone
  `.csh` compute shader), `glAttachShader`s each to a fresh
  `glCreateProgram()`, `glLinkProgram`s, and checks `GL_LINK_STATUS`.
- **`ShaderProgram`** (`citty/graphics/ShaderProgram.cpp`) wraps the linked
  program name and exposes `setUniform` overloads that resolve+cache uniform
  locations (`getUniformLocation`, lines 37-43, memoized in
  `uniformLocations`) and call the `glProgramUniform*` **DSA** variants
  (no `glUseProgram` needed before setting a uniform — `glProgramUniform*`
  targets the program by name directly), plus `dispatchCompute()`
  (line 70-74) for the light-culling compute shader.

`RenderingEngine`'s constructor builds all four programs up front
(`RenderingEngine.cpp:21-41`): `depthShaderProgram`, `lightCullingShaderProgram`
(a compute-only program — no vertex/fragment stage), `lightAccumulationShaderProgram`,
`hdrShaderProgram`.

### 4.4 Textures

`Texture` (`citty/graphics/Texture.cpp`) wraps a 2D texture object created via
`glCreateTextures(GL_TEXTURE_2D, 1, &name)` (immediate, DSA-style allocation —
no target binding required before configuring it). `Texture2D` adds two
constructors: one that allocates **immutable storage** for a render-target use
case (`SizedImageFormat`, width, height — used for the depth and HDR color
attachments, §4.5) and one that uploads a CPU-side `Image` (decoded via
`stb_image`, see `Image.cpp`) for material textures (diffuse/specular/normal/
height maps). Texture parameters (`setMinFilter`/`setMagFilter`/
`setSWrapMode`/`setTWrapMode`/`setBorderColor`) are all set with
`glTextureParameteri`/`glTextureParameterfv` (DSA, no bind), and binding for
sampling at draw/dispatch time uses `glBindTextureUnit` (`Texture.cpp:62-65`)
— the modern "bind directly to a texture unit index" entry point, replacing
the old `glActiveTexture`+`glBindTexture` two-step.

`RenderingEngine::useMaterial` (`RenderingEngine.cpp:296-315`) binds a
material's four maps to fixed texture units 0-3 (diffuse, specular, normal,
height) and sets the matching `sampler2D` uniforms to those unit indices —
the depth texture used by the light-culling compute shader is separately
bound to unit 4 (`RenderingEngine.cpp:199-200`).

### 4.5 Framebuffers & render targets

`Framebuffer` (`citty/graphics/Framebuffer.cpp`) wraps `glCreateFramebuffers`
+ `glNamedFramebufferTexture`/`glNamedFramebufferRenderbuffer` (DSA) for
attaching color/depth images, and `glNamedFramebufferDrawBuffer`/
`ReadBuffer` for controlling which attachment(s) participate in writes/reads.
`RenderingEngine::setViewportDimensions` (`RenderingEngine.cpp:138-178`,
re-run on every resize) builds two render targets sized to the new
viewport:

- **`depthFramebuffer`**: a single `Texture2D` depth attachment,
  `SizedImageFormat::DEPTH_COMPONENT24` (24-bit depth, no stencil — the
  pipeline never uses stencil testing), nearest filtering (depth values
  shouldn't be smoothed) and `CLAMP_TO_BORDER` with a border of all-`1.0`
  (maximum depth = "infinitely far"), so any sampling outside `[0,1]` UV
  (which can't actually happen given how the culling shader samples it, but
  is defensive) reads as "nothing there". `setNoDrawBuffer()`/
  `setNoReadBuffer()` (lines 162-163) disable the color attachment slot
  entirely — this is a depth-only framebuffer, matching the empty fragment
  shader (`depth.fsh`).
- **`hdrFramebuffer`**: a `Texture2D` color attachment (`SizedImageFormat::RGB8`,
  bound at color-attachment index 0, linear filtering for the later
  fullscreen-quad sample) plus a `Renderbuffer` depth attachment
  (`DEPTH_COMPONENT24` — depth testing is still needed *during* shading to
  resolve overlapping geometry correctly, but the depth values themselves
  aren't sampled afterward, so a renderbuffer — cheaper, write-only, no
  texture sampling capability — suffices instead of another `Texture2D`).

The final pass (`hdr.fsh`) reads `hdrColorTexture` and writes straight to the
**default framebuffer** (`glBindFramebuffer(GL_FRAMEBUFFER, 0)`,
`RenderingEngine.cpp:225`), which GLFW then presents via
`glfwSwapBuffers` (`GLFWRenderingSystem.cpp:53`).

### 4.6 Error checking

Every wrapper method that issues a GL call follows it with
`checkOpenGlErrors()` (`citty/graphics/OpenGlError.cpp`), which loops
`glGetError()` until `GL_NO_ERROR` and throws on the first non-zero code —
giving immediate, attributable failures at the call site instead of a stray
error silently surfacing many calls later.

---

## 5. Memory layout

### 5.1 The interleaved `Vertex` struct & the global mesh atlas

```cpp
struct Vertex {
    Eigen::Vector3f position;   // offset  0, 12 bytes
    Eigen::Vector3f normal;     // offset 12, 12 bytes
    Eigen::Vector3f tangent;    // offset 24, 12 bytes
    Eigen::Vector3f bitangent;  // offset 36, 12 bytes
    Eigen::Vector2f texCoords;  // offset 48,  8 bytes
};                              // total: 56 bytes/vertex, no padding (all float fields)
```

(`citty/graphics/Vertex.hpp`). This is an **interleaved (AoS)** layout — all
of one vertex's attributes are contiguous — the conventional choice for
static mesh data, since a vertex shader invocation typically needs every
attribute of one vertex together, making interleaved layout cache-friendly
for the GPU's vertex fetch.

Rather than one VBO per mesh, **all meshes share one global vertex buffer**
(`RenderingEngine::vertexBuffer`, `RenderingEngine.hpp:121`) and **one global
index buffer** (`indexBuffer`, line 122). `loadMesh()`
(`RenderingEngine.cpp:65-101`) `append()`s a new mesh's vertices/indices onto
the end of these buffers and records where they landed:

```cpp
struct MeshRecord {
    std::shared_ptr<Buffer<Eigen::Affine3f>> transformBuffer;
    std::vector<Eigen::Affine3f> transforms;
    VertexArray vertexArrayObject;
    std::size_t verticesOffset;  // this mesh's first vertex's index into vertexBuffer
    std::size_t indicesOffset;   // this mesh's first index's position into indexBuffer
    std::size_t indicesSize;     // index count for this mesh
};
```

(`RenderingEngine.hpp:24-31`). Drawing a specific mesh later uses
`indicesOffset` (as the byte offset passed to
`glDrawElementsInstancedBaseVertexBaseInstance`, §5.6) and `verticesOffset`
(as the VAO's vertex-buffer binding offset, `vao.bindBuffer(vertexBuffer,
meshRecord.verticesOffset)`, `RenderingEngine.cpp:76`) to address only that
mesh's slice of the shared buffers — this means a single `Buffer<Vertex>`
allocation and a single `Buffer<unsigned int>` allocation back every mesh in
the scene, rather than one tiny GL buffer object per mesh (cheaper resource
management, and crucially enables instancing across meshes that share a VAO
binding scheme without rebinding buffers between draws).

`Buffer<T>::append()` (`Buffer.hpp:76-92`) implements "grow without losing
existing data" the only way DSA allows for a buffer whose size must change:
allocate a fresh, larger temp buffer, `glCopyNamedBufferSubData` the old
contents in, `glNamedBufferSubData` the new data after it, then swap the temp
buffer in as the buffer's storage (`reallocate` + a second
`glCopyNamedBufferSubData` to move the combined data back into the
now-correctly-sized permanent buffer object) and delete the temp. This is
only used at mesh/material **load time** (not per-frame), so its
copy-heavy cost is acceptable.

### 5.2 The vertex array object's attribute bindings

`loadMesh()` configures one `VertexArray` per mesh
(`RenderingEngine.cpp:71-98`) with attribute locations matching every
shader's `layout(location = N)` declarations:

| Location | Attribute | Source buffer | Components | Notes |
|---|---|---|---|---|
| 0 | `position` | `vertexBuffer` | 3 floats | offset 0 in `Vertex` |
| 1 | `normal` | `vertexBuffer` | 3 floats | `offsetof(Vertex, normal)` |
| 2 | `tangent` | `vertexBuffer` | 3 floats | `offsetof(Vertex, tangent)` |
| 3 | `bitangent` | `vertexBuffer` | 3 floats | `offsetof(Vertex, bitangent)` |
| 4 | `texCoords` | `vertexBuffer` | 2 floats | `offsetof(Vertex, texCoords)` |
| 5,6,7,8 | `model` (`mat4`) | per-mesh `transformBuffer` | 4×(4 floats) | one `vec4` per matrix column; GLSL consumes 4 consecutive locations for a `mat4` attribute |

Locations 5-8 are the four **columns** of an instance's model matrix — GLSL
has no single-location `mat4` vertex attribute; a `mat4` input implicitly
consumes 4 consecutive attribute locations, one `vec4` each
(`layout(location = 5) in mat4 model;` in `depth.vsh:4` and
`lightAccumulation.vsh:9` — the comment "implicitly uses locations 6, 7, and
8" right below it documents exactly this). `RenderingEngine.cpp:90-97` wires
each of those 4 slots to a 16-byte stride into the `Eigen::Affine3f`'s raw
storage (`4*sizeof(float)`, `8*sizeof(float)`, `12*sizeof(float)` —
column-major storage means each successive column starts 4 floats later).

Crucially, `vao.setBufferDivisor(meshRecord.transformBuffer, 1)`
(`RenderingEngine.cpp:98`) marks the transform buffer's binding as
**per-instance** (`glVertexArrayBindingDivisor(..., 1)`): the GPU advances to
the next `Affine3f` in the buffer once per *instance* drawn, rather than once
per *vertex* — this is what makes hardware instancing work: the same mesh
geometry (locations 0-4) is reused for every instance, while location 5-8
(the model matrix) changes per instance, letting one draw call render `N`
copies of a mesh at `N` different transforms without re-submitting vertex
data or issuing `N` separate draw calls.

### 5.3 Instance transform buffer

Each `MeshRecord` owns its own `Buffer<Eigen::Affine3f>` (`transformBuffer`)
sized to (at least) the number of currently-visible instances of that mesh.
Every frame, `setGraphicsEntities()` (`RenderingEngine.cpp:235-290`) repopulates
each mesh's `transforms` vector and re-uploads it
(`transformBuffer->setSubData(transforms)`, line 288), growing the GPU buffer
(`reallocate`, 2× over-allocation to amortize regrowth, line 285-286) only
when the new instance count exceeds the current capacity. The buffer usage
hint is `STREAM_DRAW` — written once (or a few times) per frame by the CPU,
read by the GPU a handful of times, matching this every-frame-rewrite access
pattern (as opposed to `STATIC_DRAW`, used for the vertex/index buffers,
which load once and never change).

### 5.4 Shader Storage Buffer Objects (SSBOs)

Two SSBOs carry light data between the compute (culling) and fragment
(shading) stages, using GLSL's **`std430`** layout (a tighter, more
predictable packing than the older `std140`, with array element strides
exactly matching the natural C++ size/alignment of the element type for the
types used here):

**Light buffer**, `binding = 0`, `BufferUsage::STREAM_DRAW`
(`RenderingEngine.hpp:102-103`):

```glsl
struct PointLight {
    vec4 position;   // 16 bytes — w component unused, present to satisfy std430 vec4 alignment
    vec4 color;       // 16 bytes
    float radius;      // 4 bytes
};                      // 36 bytes raw, but std430 rounds each array element up to its largest
                        // member's alignment (16 bytes) → 48 bytes per array element
layout(std430, binding = 0) readonly buffer LightBuffer { PointLight lights[]; };
```

This matches `PointLightEntity`
(`citty/graphics/PointLightEntity.hpp` — `Vector4f position; Vector4f color;
float radius;`) field-for-field; `setPointLightEntities`
(`RenderingEngine.cpp:332-344`) uploads the `std::span<PointLightEntity>`
directly as raw bytes via `Buffer<PointLightEntity>::setSubData`. Using
`Vector4f` (not `Vector3f`) for position/color in the *C++* struct, even
though only 3 components are meaningful, is precisely what makes the C++
struct's memory layout match GLSL's `std430` 16-byte-aligned `vec4` padding
without manual padding fields.

**Visible-light-index buffer**, `binding = 1`, sized
`1024 * numberOfTiles` `int`s (`RenderingEngine.cpp:145-146`, recomputed on
every resize since `numberOfTiles` depends on viewport size):

```glsl
layout(std430, binding = 1) buffer VisibleLightIndicesBuffer { int indices[]; };
```

Conceptually this is a flattened 2D array: tile `index`'s light-index list
occupies the contiguous range `[index*1024, index*1024+1024)` (the culling
shader computes `offset = index * 1024` at `lightCulling.csh:142`, and the
shading shader recomputes the identical `offset` from the fragment's own
screen position at `lightAccumulation.fsh:71` — there's no header/table
of per-tile offsets; tile `index` maps to its slice by simple arithmetic).
Each tile's slice holds up to 1024 light indices, terminated early by a
sentinel `-1` if fewer than 1024 lights are visible
(`lightCulling.csh:147-151`) — this lets the shading pass's read loop stop
as soon as it sees `-1` instead of needing the true light count passed
through (`lightAccumulation.fsh:72`: `for (i = 0; i < 1024 &&
visibleLightIndicesBuffer.indices[offset+i] != -1; i++)`).

Both SSBOs are bound to their fixed binding indices fresh each pass
(`pointLightsBuffer->bindToTarget(0, ...)`,
`visiblePointLightIndexBuffer->bindToTarget(1, ...)` — called both before the
culling dispatch, `RenderingEngine.cpp:202-204`, and before the shading draw
calls, lines 219-221, since binding state isn't otherwise guaranteed to
persist across the depth-pass / `glBindFramebuffer` calls in between), and
`glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)`
(`RenderingEngine.cpp:208`) is issued right after the compute dispatch — a
hard requirement, since otherwise the fragment shader's reads from the index
buffer could race ahead of the compute shader's (incoherent, unordered)
writes; the barrier guarantees every invocation's `writeonly`/`readonly`
SSBO writes from the dispatched compute pass are visible to subsequent draw
calls.

### 5.5 Batching and draw-call construction

`GraphicsEntity::key()` (`citty/graphics/GraphicsEntity.hpp:15-17`) packs
`(materialId, meshId)` into one sortable integer: `(materialId << 16) |
meshId`. `setGraphicsEntities()` (`RenderingEngine.cpp:235-290`) sorts every
visible entity by this key, so entities sharing a material **and** mesh
become contiguous runs, then walks the sorted list accumulating each run's
length into a `RenderCommand`:

```cpp
struct RenderCommand {
    std::size_t instanceCount{};
    std::size_t offset{};      // offset into that mesh's transform buffer
    std::size_t materialId{};
    std::size_t meshId{};
};
```

(`RenderingEngine.hpp:75-80`). Because the sort key shifts `materialId` into
the high bits, the resulting `renderCommands` list is **primarily grouped by
material, secondarily by mesh** — which is exactly the grouping
`issueDrawCommands()` (`RenderingEngine.cpp:346-361`) wants: it walks the
list calling `useMaterial()` (rebinding 4 texture units + 3 uniforms,
§4.4) only when `materialId` actually changes between consecutive commands,
minimizing redundant texture/uniform state changes — then for every command
issues one **instanced indexed draw call**:

```cpp
vao.drawElementsInstanced(DrawMode::TRIANGLES, meshRecord.indicesSize,
                           drawCommand.instanceCount,
                           meshRecord.indicesOffset, /*baseVertex=*/0,
                           drawCommand.offset);
```

→ `glDrawElementsInstancedBaseVertexBaseInstance` (`VertexArray.cpp:51-59`):
one GPU command renders `instanceCount` copies of one mesh's geometry, each
instance reading its own model matrix starting at `baseInstance =
drawCommand.offset` into that mesh's `transformBuffer` (thanks to the
divisor-1 binding from §5.2). This is the mechanism that lets, e.g., five
identical car-body sub-meshes (one per car entity in the demo scene,
`citty.cpp:50-55`) be drawn with a **single** draw call instead of five.

---

## 6. The frame, pass by pass

This walks `RenderingEngine::render()` (`RenderingEngine.cpp:180-233`)
top to bottom, tying every line back to the math and memory layout above.

### 6.0 Before rendering: data refresh

`GLFWRenderingSystem::render()` (`GLFWRenderingSystem.cpp:68-75`) runs first
each frame:
1. `processLoadingQueues()` — drains any pending mesh/material/texture loads
   queued by other threads (§3.4), actually issuing the GL upload calls.
2. `uploadGraphicsEntities()` / `uploadPointLightEntities()` — if the engine
   thread produced a new snapshot since the last frame (§3.3), push it into
   `RenderingEngine` via `setGraphicsEntities`/`setPointLightEntities`, which
   re-sorts/re-batches (§5.5) and re-uploads the SSBO/instance data (§5.3,
   §5.4).
3. `renderingEngine->render()` — the four GPU passes below.

If `renderCommands` is empty (nothing to draw yet — e.g. before the first
model finishes loading), `render()` returns immediately
(`RenderingEngine.cpp:181-183`).

### 6.1 Pass 1 — Depth prepass

```cpp
depthShaderProgram.use();
depthShaderProgram.setUniform("projection", projection.matrix());
depthShaderProgram.setUniform("view", view.matrix());
depthFramebuffer.bind();
glClear(GL_DEPTH_BUFFER_BIT);
issueDrawCommands();
```

(`RenderingEngine.cpp:185-191`). Every opaque draw command (§5.5) is rendered
with `depth.vsh`/`depth.fsh` — the vertex shader does the standard
object→clip transform (§2.4) and nothing else; the fragment shader is
*empty* (no color output at all — recall `depthFramebuffer` has no draw
buffer, §4.5). The only side effect is populating the **depth buffer** with
each pixel's nearest-surface depth, via the GPU's standard depth test
(enabled globally, §4.1) — this is the texture the next pass reads to know,
per tile, the range of depths actually present in the scene (as opposed to
the full near/far range of the whole frustum, which would make every tile's
frustum span the entire camera depth range and defeat the purpose of culling).
Why is this a *separate* pass rather than reusing the depth buffer the
shading pass naturally produces? Because culling (pass 2) must run *before*
shading (pass 3) can know which lights apply to a tile — the dependency
order is depth → cull → shade, so depth has to be available first,
standalone.

### 6.2 Pass 2 — Light culling (compute)

```cpp
glBindFramebuffer(GL_FRAMEBUFFER, 0);
lightCullingShaderProgram.use();
lightCullingShaderProgram.setUniform("lightCount", pointLightCount);
lightCullingShaderProgram.setUniform("projection", projection.matrix());
lightCullingShaderProgram.setUniform("view", view.matrix());
depthTexture->bindToTextureUnit(4);
lightCullingShaderProgram.setUniform("depthMap", 4);
pointLightsBuffer->bindToTarget(0, BufferTarget::SHADER_STORAGE_BUFFER);
visiblePointLightIndexBuffer->bindToTarget(1, BufferTarget::SHADER_STORAGE_BUFFER);
lightCullingShaderProgram.dispatchCompute(lightCullingWorkgroupsX, lightCullingWorkgroupsY, 1);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
Texture::unbindTextureUnit(4);
```

(`RenderingEngine.cpp:193-210`). One workgroup is dispatched per 16×16-pixel
screen tile (`lightCullingWorkgroupsX/Y`, computed as `ceil(width/16)` /
`ceil(height/16)` whenever the viewport resizes,
`RenderingEngine.cpp:142-144`), each workgroup running 256 threads
(`local_size_x = local_size_y = TILE_SIZE = 16`, `lightCulling.csh:39`).
Within `lightCulling.csh`'s `main()`:

1. **Setup** (lines 48-53): thread 0 of each workgroup resets the shared
   `minDepthInt`/`maxDepthInt`/`visibleLightCount` accumulators and computes
   `viewProjection = projection * view` once for the whole tile, then a
   `barrier()` (line 55) ensures every thread in the workgroup sees these
   initial values before continuing (GLSL compute shaders need explicit
   barriers — there's no implicit ordering between invocations in a
   workgroup beyond what `barrier()`/`memoryBarrierShared()` impose).
2. **Per-pixel depth reduction** (lines 57-69): every thread (one per pixel
   in the tile) samples `depthTexture` at its own pixel, linearizes it
   (§2.11's derivation), and folds it into the shared min/max via
   `atomicMin`/`atomicMax` on the float's bit pattern.
3. **Frustum plane construction** (lines 72-100, thread 0 only): builds the
   six world-space planes from the tile's NDC bounds and the depth
   min/max, as derived in §2.11.
4. **Parallel light testing** (lines 104-136): the 256 threads in the
   workgroup divide up the `lightCount` lights (`passCount =
   ceil(lightCount / 256)` rounds of work so > 256 lights are still handled,
   just in multiple passes per thread) and each runs the 6-plane
   sphere-frustum test (§2.11) on its assigned light(s), appending its index
   to the shared `visibleLightIndices` array via `atomicAdd(visibleLightCount,
   1)` (a lock-free shared append — each thread atomically claims a unique
   slot index before writing, so concurrent writes from different threads
   never collide).
5. **Write-out** (lines 140-152, thread 0 only): copies the workgroup-local
   `visibleLightIndices` into this tile's slice of the global
   `visibleLightIndicesBuffer` (§5.4), appending the `-1` sentinel if the
   tile saw fewer than 1024 visible lights.

The `glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)` after dispatch (§5.4)
is what makes it safe for the *next* pass to read this buffer.

### 6.3 Pass 3 — Light accumulation (shading)

```cpp
hdrFramebuffer.bind();
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
lightAccumulationShaderProgram.use();
lightAccumulationShaderProgram.setUniform("projection", projection.matrix());
lightAccumulationShaderProgram.setUniform("view", view.matrix());
lightAccumulationShaderProgram.setUniform("viewPosition", viewPosition);
pointLightsBuffer->bindToTarget(0, BufferTarget::SHADER_STORAGE_BUFFER);
visiblePointLightIndexBuffer->bindToTarget(1, BufferTarget::SHADER_STORAGE_BUFFER);
issueDrawCommands();
```

(`RenderingEngine.cpp:212-223`). This re-issues the *exact same* batched
draw commands as the depth prepass (§5.5), now against the real shading
program, rendering into the `hdrFramebuffer`'s floating/8-bit color
attachment with depth testing active (so overlapping geometry still resolves
correctly even though the depth buffer here starts fresh — `GL_DEPTH_BUFFER_BIT`
is cleared, this is a *different* depth buffer than pass 1's, attached to a
different framebuffer, §4.5). Per draw command, `useMaterial()` binds the
four material textures and material uniforms (§4.4) before the GPU executes:

- **Vertex stage** (`lightAccumulation.vsh`): standard object→clip transform
  (§2.4) for `gl_Position`; computes world-space `fragmentPosition`; builds
  the per-vertex `TBN` matrix from the inverse-transpose-corrected
  tangent/bitangent/normal (§2.8, §2.9) and pre-transforms the view
  position and fragment position into tangent space for the fragment stage.
- **Fragment stage** (`lightAccumulation.fsh`):
  1. Recomputes which 16×16 tile this fragment belongs to from
     `gl_FragCoord.xy` (lines 49-51) — the *same* tile-index formula the
     culling pass used, so this fragment's lookup into
     `visibleLightIndicesBuffer` lines up with the slice that pass wrote.
  2. Samples diffuse/specular/normal maps (§2.9, §2.10), discards fully
     transparent fragments (`base_diffuse.a <= 0.2`, line 57-59 — a cheap
     alpha-test cutout, not true transparency/blending) before doing any
     further work.
  3. Loops only over this tile's visible light indices (not all scene
     lights — the entire point of pass 2), accumulating
     Blinn-Phong diffuse+specular with the custom radius-based attenuation
     for each (§2.10, §2.10b).
  4. Adds the flat ambient term and writes the (potentially > 1.0, since
     nothing here clamps yet) HDR `fragColor`.

### 6.4 Pass 4 — HDR resolve / tone mapping

```cpp
glBindFramebuffer(GL_FRAMEBUFFER, 0);
hdrShaderProgram.use();
hdrColorTexture->bindToTextureUnit(0);
hdrShaderProgram.setUniform("hdrBuffer", 0);
hdrShaderProgram.setUniform("exposure", 1.0f);
glDisable(GL_DEPTH_TEST);
hdrQuadVAO.draw(DrawMode::TRIANGLE_STRIP, 4);
glEnable(GL_DEPTH_TEST);
```

(`RenderingEngine.cpp:225-232`). Targets the default framebuffer (the
window). `hdrQuadVAO` is a tiny static, non-indexed VAO of 4 vertices in
**NDC space already** (`QuadVertex{position, texCoords}`, set up once in the
constructor, `RenderingEngine.cpp:47-58`) covering `[-1,1]²` — `hdr.vsh`
(lines 9-11) passes `position` straight through to `gl_Position` with no
model/view/projection multiply at all, since a fullscreen pass needs no 3D
transform, just raw clip-space coordinates. Drawn as a `TRIANGLE_STRIP` of 4
vertices (two triangles sharing an edge, covering the full screen
quad — `(-1,1),(-1,-1),(1,1),(1,-1)` strip-winds into two triangles). Depth
testing is disabled for this draw (`glDisable(GL_DEPTH_TEST)`) since this
quad must unconditionally overwrite every pixel of the previous frame's
leftover depth/color in the default framebuffer, then immediately
re-enabled afterward so the *next* frame's depth prepass starts from a clean
state with depth testing active again.

The fragment shader (`hdr.fsh`) samples `hdrBuffer` at this fragment's
texture coordinate, applies the Reinhard tone-map and gamma correction
derived in §2.12, and writes the final, display-ready `fragColor` — the last
step in the pipeline.

---

## 7. Equation cheat sheet

| Concept | Equation | Where |
|---|---|---|
| Homogeneous point/vector | `(x,y,z,1)` / `(x,y,z,0)` | §2.1 |
| Model matrix | `M = T · R · S` | §2.2, `GLFWRenderingSystem.cpp:240` |
| World matrix (parented) | `M_world = M_world(parent) · M_local(child)` | §2.2 |
| GLSL↔Eigen indexing | `glsl[col][row] ≡ eigen(row, col)` | §2.3 |
| Perspective X/Y scale | `P(0,0)=1/(aspect·tan(fovY/2))`, `P(1,1)=1/tan(fovY/2)` | §2.7 |
| Perspective Z mapping | `A=-(f+n)/(f-n)`, `B=-2fn/(f-n)`, `clip.z=Az+B`, `clip.w=-z` | §2.7 |
| Depth → linear view-z | `-z = 0.5·B / (depthBuf + 0.5·A - 0.5)` | §2.11 |
| Normal transform | `N' = (M⁻¹)ᵀ N` | §2.8 |
| Tangent→view, world→tangent | `TBN = [T B N]`, world→tangent `= TBNᵀ` | §2.9 |
| Lambert diffuse | `diffuse = max(N·L, 0)` | §2.10 |
| Blinn-Phong specular | `H = normalize(L+V)`, `specular = max(N·H,0)^shininess` | §2.10 |
| Combined irradiance | `color += Lc·(Kd·diffuse + Ks·specular)·A(d)` | §2.10 |
| Custom attenuation | `A(d) = clamp((1/(0.15·d²/r + 1) − 0.5)/0.5, 0, 1)` | §2.10b |
| Plane / signed distance | `plane·(p,1) = n·p + d` | §2.11 |
| Plane transform (clip→world) | `p_world = p_clip · ViewProjection` (row-vector form) | §2.11 |
| Sphere-frustum test | intersects iff `∀ planes: n·P + d + r > 0` | §2.11 |
| Reinhard tone map | `result = 1 − e^(−color·exposure)` | §2.12 |
| Gamma correction | `result = result^(1/2.2)` | §2.12 |
