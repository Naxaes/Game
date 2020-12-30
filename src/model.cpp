#include "model.h"

#include <cstddef>

#include "maths.h"


Mesh CreateMesh(const std::vector<Vertex>& vertices)
{
    // Create vertex array buffer to store vertex buffers and element buffers.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer to put our data into video memory.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Tell OpenGL the data's format.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv_coord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    return { vao, vertices.size() };
}


Mesh CreateMesh(const vec3* positions, const vec2* uv_coords, const vec3* normals, size_t vertex_count)
{
    size_t position_size = vertex_count * sizeof(*positions);
    size_t texture_size  = vertex_count * sizeof(*uv_coords);
    size_t normal_size   = vertex_count * sizeof(*normals);
    size_t total_size    = position_size + texture_size + normal_size;

    // Create vertex array buffer to store vertex buffers and element buffers.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer to put our data into video memory.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Allocate a buffer and then insert data.
    glBufferData(GL_ARRAY_BUFFER, total_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, position_size, positions);
    glBufferSubData(GL_ARRAY_BUFFER, position_size, texture_size,  uv_coords);
    glBufferSubData(GL_ARRAY_BUFFER, position_size + texture_size, normal_size, normals);

    // Tell OpenGL the data's format.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*positions), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*uv_coords), (void *) position_size);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*normals),   (void *) (position_size + texture_size));

    return { vao, vertex_count };
}



Mesh CreateMesh(const std::vector<float>& positions, const std::vector<float>& texture_coordinates, const std::vector<float>& normals, const std::vector<GLuint>& indices)
{
    // TODO(ted): Maybe use some of these tips https://www.khronos.org/opengl/wiki/VertexSpecification_Best_Practices.

    size_t position_size = positions.size();
    size_t texture_size  = texture_coordinates.size();
    size_t normal_size   = normals.size();
    size_t total_size    = position_size + texture_size + normal_size;

    // Create vertex array buffer to store vertex buffers and element buffers.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create an element buffer to put our data into video memory.
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Create vertex buffer to put our data into video memory.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Allocate a buffer and then insert data.
    glBufferData(GL_ARRAY_BUFFER, total_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, position_size, positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, position_size, texture_size,  texture_coordinates.data());
    glBufferSubData(GL_ARRAY_BUFFER, position_size + texture_size, normal_size, normals.data());

    // Tell OpenGL the data's format.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) position_size);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) (position_size + texture_size));

    return { vao, indices.size() };
}