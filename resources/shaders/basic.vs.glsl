#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;

out vec2 out_uv_coord;

uniform mat4 model;

void main()
{
    gl_Position  = model * vec4(position.x, position.y, position.z, 1.0);
    out_uv_coord = uv_coord;
}