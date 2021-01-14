#version 330 core

//in vec3  out_position;
in vec2  out_uv_coord;
in vec3  out_normal;
in float out_texture_index;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;

out vec4 FragColor;

void main()
{
    int index = int(out_texture_index);

    vec4 color = vec4(0);
    if (index == 0)
        color = texture(texture0, out_uv_coord);
    else if (index == 1)
        color = texture(texture1, out_uv_coord);
    else if (index == 2)
        color = texture(texture2, out_uv_coord);
    else if (index == 3)
        color = texture(texture3, out_uv_coord);
    else if (index == 4)
        color = texture(texture4, out_uv_coord);
    else if (index == 5)
        color = texture(texture5, out_uv_coord);

    FragColor = color;
}
