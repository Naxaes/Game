#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
