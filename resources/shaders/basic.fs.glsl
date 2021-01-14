#version 330 core

in vec3 out_position;
in vec2 out_uv_coord;
in vec3 out_normal;

uniform sampler2D diffuse;

out vec4 FragColor;

void main()
{
    vec4 color = texture(diffuse, out_uv_coord);
    FragColor  = color;
}
