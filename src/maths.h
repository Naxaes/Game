#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::vec2;


struct Vertex
{
    vec3 position;
    vec2 uv_coord;
    vec3 normal;
};