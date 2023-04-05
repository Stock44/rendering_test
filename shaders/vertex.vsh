#version 460 core
// per vertex data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 texCoords;

layout (location = 5) in mat4 model;
// implicitly uses locations 6, 7 and 8

out vec4 color;

uniform mat4 view;
uniform vec3 diffuse;
uniform vec3 specular;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    color = vec4(1.0);
}
