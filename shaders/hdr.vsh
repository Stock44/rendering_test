#version 450 core

layout(location = 0) in vec3 position;

layout(location = 4) in vec2 textureCoordinates;

out vec2 TextureCoordinates;

void main() {
    gl_Position = vec4(position, 1.0);
    TextureCoordinates = textureCoordinates;
}