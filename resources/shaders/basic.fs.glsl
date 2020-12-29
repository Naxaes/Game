#version 330 core

in vec2 out_uv_coord;

uniform vec3 object_color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(object_color, 1.0f);
}
