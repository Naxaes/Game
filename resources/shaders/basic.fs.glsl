#version 330 core

in vec3 out_position;
in vec2 out_uv_coord;
in vec3 out_normal;

uniform vec3 object_color;
uniform sampler2D diffuse;

out vec4 FragColor;

void main()
{
    vec4 color = texture(diffuse, out_uv_coord);
    FragColor  = vec4(object_color * 0.1f + color.xyz * 0.9f, 1.0f);
}
