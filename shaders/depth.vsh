#version 460 core

layout (location = 0) in vec3 position;
layout (location = 5) in mat4 model;
// implicitly uses locations 6, 7 and 8

// Uniforms
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
}
