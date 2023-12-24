#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 color;
uniform int colorMode;

vec4 Black = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 Gray = vec4(0.5f, 0.5f, 0.5f, 1.0f);
vec4 White = vec4(1.0f, 1.0f, 1.0f, 1.0f); 

void main()
{
    FragColor = color;
}
