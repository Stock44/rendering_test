#version 460 core
out vec4 FragColor;

in vec4 color;

//uniform sampler2D ourTexture;

void main()
{
    FragColor = color;
}