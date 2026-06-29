# citty

An OpenGL 4.5 renderer built in C++23 using GLFW and a custom Entity-Component-System (ECS) framework. Implements tile-based deferred rendering with GPU-side light culling, normal mapping, instanced geometry, and HDR tone mapping.

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
  - [ECS Engine](#ecs-engine)
  - [Rendering Pipeline](#rendering-pipeline)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building](#building)
- [Running](#running)
- [Shaders](#shaders)

---

## Overview

citty is a rendering engine that loads and displays 3D models (via Assimp) under a dynamic point lighting system. The renderer uses a forward+ style pipeline: a depth prepass feeds a compute-shader light culler that bins visible lights per screen tile, then a shading pass reads only the relevant lights for each fragment. Post-processing applies Reinhard HDR tone mapping with gamma correction.

The demo scene contains procedurally defined cube and pyramid meshes as well as a loaded OBJ model (`assets/car.obj`), all lit by a configurable point light. Entities spin over time to exercise the transform hierarchy.

---

## Architecture

### ECS Engine

The engine (`citty/engine/`) implements an archetype-based ECS:

- **Archetypes** are unique signatures of component types. Entities with the same set of components share an archetype, enabling tight cache-friendly iteration.
- **`ArchetypeGraph`** tracks valid transitions when components are added or removed from an entity, using Boost.Graph internally.
- **`ComponentStore`** owns all component data, partitioned by archetype into `ArchetypeRecord` tables.
- **`Entity`** is a lightweight handle that provides typed `addComponent` / `getComponent` access.
- **`System`** is a base class that systems derive from; the `Engine` calls `update()` on each registered system each frame.
- **`Transform`** supports a parent-child hierarchy: child transforms are resolved relative to their parent at render time.

```
Engine
├── EntityIdStore       — allocates/recycles entity IDs
├── ComponentStore      — stores components grouped by archetype
│   └── ArchetypeRecord — column-oriented component table per archetype
├── ArchetypeGraph      — valid archetype transitions
└── System[]            — updated each frame
```

### Rendering Pipeline

The renderer (`citty/graphics/`) runs four sequential passes each frame:

```
1. Depth Prepass
   └── Renders the scene to a depth texture (depth.vsh / depth.fsh)

2. Light Culling  [compute shader]
   ├── Divides screen into 16×16 pixel tiles
   ├── Each workgroup reads the tile's depth min/max from the depth texture
   ├── Constructs 6 frustum planes for the tile
   ├── Tests every point light's bounding sphere against the frustum
   └── Writes visible light indices into a Shader Storage Buffer (SSBO)
       (lightCulling.csh)

3. Shading Pass
   ├── Transforms normals with a TBN matrix for normal mapping
   ├── For each fragment, reads only lights visible in its tile from the SSBO
   ├── Blinn-Phong per light: diffuse + specular + attenuation
   ├── 8% ambient term
   └── Outputs HDR colour to a floating-point framebuffer
       (lightAccumulation.vsh / lightAccumulation.fsh)

4. HDR Tone Mapping
   ├── Reinhard operator:  1 - exp(-colour × exposure)
   ├── Gamma correction:   pow(result, 1/2.2)
   └── Outputs to the default framebuffer
       (hdr.vsh / hdr.fsh)
```

**Instanced rendering** — all entities sharing the same mesh are batched into a single draw call. Per-entity model matrices are uploaded via a GPU buffer updated each frame.

**Resource management** — meshes, materials, and textures are identified by integer handles. Loading is done off the render thread using `std::promise`/`std::future`; the render thread only consumes the ready resources.

---

## Project Structure

```
citty/
├── engine/                    # ECS framework
│   ├── components/
│   │   └── Transform.hpp      # Position, rotation, scale + parent hierarchy
│   ├── Archetype.hpp
│   ├── ArchetypeGraph.{cpp,hpp}
│   ├── ArchetypeRecord.{cpp,hpp}
│   ├── Component.hpp
│   ├── ComponentContainer.{cpp,hpp}
│   ├── ComponentStore.{cpp,hpp}
│   ├── Engine.{cpp,hpp}
│   ├── Entity.hpp
│   ├── EntityId.hpp
│   ├── EntityIdStore.{cpp,hpp}
│   ├── System.{cpp,hpp}
│   ├── ArchetypeGraph.test.cpp
│   └── ArchetypeRecord.test.cpp
│
├── graphics/                  # Rendering subsystem
│   ├── components/
│   │   ├── Camera.hpp
│   │   ├── Graphics.hpp       # Mesh + material handle for renderable entities
│   │   ├── MeshFile.hpp
│   │   └── PointLight.hpp     # Colour + radius
│   ├── Buffer.hpp             # Type-safe GPU buffer (VBO, SSBO, …)
│   ├── Framebuffer.{cpp,hpp}
│   ├── GLFWRenderingSystem.{cpp,hpp}   # GLFW entry point + render loop
│   ├── GraphicsEntity.hpp
│   ├── Image.{cpp,hpp}        # stb_image wrapper
│   ├── Material.hpp           # Diffuse/specular colours + texture handles
│   ├── Math.{cpp,hpp}         # Projection & view matrix helpers
│   ├── Mesh.hpp               # CPU-side vertex + index data
│   ├── Model.hpp              # Hierarchical model (nodes + meshes)
│   ├── OpenGlError.{cpp,hpp}
│   ├── PointLightEntity.hpp
│   ├── Renderbuffer.{cpp,hpp}
│   ├── RenderingEngine.{cpp,hpp}   # Core draw commands
│   ├── RenderingSystem.{cpp,hpp}   # GTK GLArea integration
│   ├── Shader.{cpp,hpp}
│   ├── ShaderProgram.{cpp,hpp}
│   ├── ShaderProgramBuilder.{cpp,hpp}
│   ├── SizedImageFormat.hpp
│   ├── Texture.{cpp,hpp}
│   ├── Texture2D.{cpp,hpp}
│   ├── TextureSettings.hpp
│   ├── Vertex.hpp             # {position, normal, tangent, bitangent, texCoords}
│   └── VertexArray.{cpp,hpp}
│
├── citty.cpp                  # Entry point — GTK GLArea backend
└── citty-graphics-only.cpp    # Entry point — pure GLFW backend

shaders/
├── depth.vsh / depth.fsh          # Pass 1: depth prepass
├── lightCulling.csh               # Pass 2: tile-based light culling (compute)
├── lightAccumulation.vsh / .fsh   # Pass 3: Blinn-Phong + normal mapping
├── hdr.vsh / hdr.fsh              # Pass 4: tone mapping + gamma correction
└── old.vsh                        # Legacy / unused

assets/
└── car.obj / car.mtl              # Demo car model

resources/
└── no_texture.png                 # Fallback texture
```

---

## Dependencies

| Library | Role | Version |
|---|---|---|
| **GLFW** | Window and OpenGL context creation | `^3.3.8` |
| **epoxy** | OpenGL function loader | `^1.5.10` |
| **gtkmm-4.0** | GTK backend (alternative entry point) | `^4.8.0` |
| **Eigen** | Vector/matrix/quaternion math | `^3.4.0` |
| **Assimp** | 3D model import (OBJ + others) | `^5.2.0` |
| **stb_image** | PNG/JPG texture loading | `^2.26.0` |
| **Boost.Graph** | Archetype transition graph | `~1.78.0` |
| **Boost.Flyweight** | Immutable archetype reuse | `~1.78.0` |
| **Boost.Hana** | Compile-time metaprogramming | `~1.78.0` |
| **pugixml** | XML parsing (map loading) | `^1.12.1` |
| **Catch2** | Unit tests | `^3.3.2` |

---

## Building

The project uses the [build2](https://build2.org) toolchain.

```sh
# Configure and build (GLFW-only executable)
bdep init
b citty/exe{citty-graphics-only}

# Build the GTK variant
b citty/exe{citty}

# Build and run unit tests
b citty/exe{tests}
./citty/tests
```

Requires a C++23-capable compiler (GCC 13+ or Clang 16+) and OpenGL 4.5.

---

## Running

```sh
# GLFW backend (recommended for standalone use)
./citty/citty-graphics-only

# GTK backend
./citty/citty
```

The demo scene opens a 640×480 window displaying:

- A **cube** at `(10, 0, 0)` spinning around the Y axis.
- A **pyramid** parented to the cube, offset `(0, 0, -5)` in local space.
- A second **cube** parented to the pyramid at `(0, 2, 0)`.
- The **car model** (`assets/car.obj`) at `(10, -3, 0)`, also rotating.
- A **point light** (white, radius 20) at `(7, 0, 0)`.

---

## Shaders

### `depth.vsh` / `depth.fsh`
Minimal vertex transformation; empty fragment shader. Fills the depth buffer used by the culling pass.

### `lightCulling.csh`
Compute shader dispatched once per tile (16×16 threads per workgroup).

1. Reads per-pixel depth from the depth texture; computes tile depth min/max with atomic operations.
2. Constructs 6 view-frustum planes for the tile from the depth bounds.
3. Parallelises light testing across threads; performs sphere-frustum intersection for each point light.
4. Writes a sentinel-terminated list of visible light indices into an SSBO (max 1024 per tile), based on guidance from [DICE's Battlefield 3 deferred renderer](http://www.dice.se/news/directx-11-rendering-battlefield-3/).

### `lightAccumulation.vsh` / `lightAccumulation.fsh`
Vertex stage builds a TBN matrix and outputs world-space position/normal for the fragment stage.

Fragment stage:
- Reads the normal map and converts from `[0,1]` to `[-1,1]`.
- Looks up which tile the fragment belongs to and reads its visible light list from the SSBO.
- Per light: Blinn-Phong diffuse + specular with a smooth distance attenuation based on the light's radius.
- Adds a fixed 8% ambient contribution.

### `hdr.vsh` / `hdr.fsh`
Full-screen quad pass. Applies `1 - exp(-colour × exposure)` (Reinhard) then `pow(result, 1/2.2)` gamma correction before writing to the default framebuffer.
