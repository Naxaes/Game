#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_position;
out vec2 out_uv_coord;
out vec3 out_normal;

void main()
{
    out_position = vec3(model * vec4(position, 1.0f));
    out_uv_coord = uv_coord;

    out_normal = vec3(model * vec4(normal, 0.0f));

    gl_Position = projection * view * vec4(out_position, 1.0f);
}
