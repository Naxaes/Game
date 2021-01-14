#version 330 core

layout (location = 0) in vec3  position;
layout (location = 1) in vec2  uv_coord;
layout (location = 2) in vec3  normal;
layout (location = 3) in float texture_index;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

out vec3  out_position;
out vec2  out_uv_coord;
out vec3  out_normal;
out float out_texture_index;

void main()
{
//    out_position = position;
    out_uv_coord = uv_coord;
    out_normal   = normal;
    out_texture_index = texture_index;

    gl_Position = vec4(position, 1.0f);
}
