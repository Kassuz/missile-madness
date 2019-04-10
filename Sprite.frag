#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sprite;
uniform vec4 spriteColor;

void main()
{
    FragColor = texture(sprite, TexCoord) * spriteColor;
}