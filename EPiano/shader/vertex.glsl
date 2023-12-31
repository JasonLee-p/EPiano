#version 330 core

layout (location = 0) in vec2 position;
out vec2 TexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position.x, position.y, 0.0, 1.0);
    TexCoord = position;
}
