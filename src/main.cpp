#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "window.h"
#include "shader.h"
#include "utils.h"


using glm::vec2;
using glm::vec3;

const vec4 RED     = vec4(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 GREEN   = vec4(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 BLUE    = vec4(0.0f, 0.0f, 1.0f, 1.0f);
const vec4 YELLOW  = vec4(1.0f, 1.0f, 0.0f, 1.0f);
const vec4 MAGENTA = vec4(1.0f, 0.0f, 1.0f, 1.0f);
const vec4 CYAN    = vec4(0.0f, 1.0f, 1.0f, 1.0f);
const vec4 WHITE   = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 BLACK   = vec4(0.0f, 0.0f, 0.0f, 0.0f);


struct Cube
{
    static constexpr vec3 VERTICES[] = {
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {-1.0f,  1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0}
    };

    static constexpr vec2 UV_COORDS[] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},

        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},

        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},

        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},

        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},

        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f}
    };

    static constexpr vec3 NORMALS[] = {
            {0.0f,  0.0f, -1.0f},
            {0.0f,  0.0f, -1.0f},
            {0.0f,  0.0f, -1.0f},
            {0.0f,  0.0f, -1.0f},
            {0.0f,  0.0f, -1.0f},
            {0.0f,  0.0f, -1.0f},

            {0.0f,  0.0f,  1.0f},
            {0.0f,  0.0f,  1.0f},
            {0.0f,  0.0f,  1.0f},
            {0.0f,  0.0f,  1.0f},
            {0.0f,  0.0f,  1.0f},
            {0.0f,  0.0f,  1.0f},

            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},

            {1.0f,  0.0f,  0.0f},
            {1.0f,  0.0f,  0.0f},
            {1.0f,  0.0f,  0.0f},
            {1.0f,  0.0f,  0.0f},
            {1.0f,  0.0f,  0.0f},
            {1.0f,  0.0f,  0.0f},

            {0.0f, -1.0f,  0.0f},
            {0.0f, -1.0f,  0.0f},
            {0.0f, -1.0f,  0.0f},
            {0.0f, -1.0f,  0.0f},
            {0.0f, -1.0f,  0.0f},
            {0.0f, -1.0f,  0.0f},

            {0.0f,  1.0f,  0.0f},
            {0.0f,  1.0f,  0.0f},
            {0.0f,  1.0f,  0.0f},
            {0.0f,  1.0f,  0.0f},
            {0.0f,  1.0f,  0.0f},
            {0.0f,  1.0f,  0.0f}
    };
};


struct Position { float x, y, z; };
struct Rotation { float x, y, z; };
struct Transform
{
    Position position;
    Rotation velocity;
    float  scale;
};

struct Mesh
{
    GLuint id;
    size_t count;
};
struct Renderable
{
     vec3  color;
     Mesh* mesh;
};



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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*positions),           (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*uv_coords), (void *) position_size);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*normals),             (void *) (position_size + texture_size));

    return { vao, vertex_count };
}


int main()
{
    Window window = CreateWindow(2880, 1710);

    auto shader = CreateShader("Basic", LoadFileToString("../resources/shaders/basic.vs.glsl").get(), LoadFileToString("../resources/shaders/basic.fs.glsl").get());
    auto mesh   = CreateMesh(Cube::VERTICES, Cube::UV_COORDS, Cube::NORMALS, 36);

    entt::registry registry;
    for(const auto color : { RED })
    {
        const auto entity = registry.create();
        registry.emplace<Transform>(entity, Position{0,0,-3}, Rotation{0,0,0}, 0.2f);
        registry.emplace<Renderable>(entity, color, &mesh);
    }

    while (!glfwWindowShouldClose(window.id))
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader.id);
        for(auto [entity, transform, renderable]: registry.view<const Transform, const Renderable>().each())
        {
            glBindVertexArray(renderable.mesh->id);
            SetUniform(shader, "object_color", renderable.color);
            glDrawArrays(GL_TRIANGLES, 0, renderable.mesh->count);
        }
        glUseProgram(0);

        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    return 0;
}
