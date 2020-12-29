#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

#include "window.h"
#include "shader.h"
#include "utils.h"
#include "camera.h"


using glm::vec2;
using glm::vec3;

static constexpr vec3 RED     {1.0f, 0.0f, 0.0f};
static constexpr vec3 GREEN   {0.0f, 1.0f, 0.0f};
static constexpr vec3 BLUE    {0.0f, 0.0f, 1.0f};
static constexpr vec3 YELLOW  {1.0f, 1.0f, 0.0f};
static constexpr vec3 MAGENTA {1.0f, 0.0f, 1.0f};
static constexpr vec3 CYAN    {0.0f, 1.0f, 1.0f};
static constexpr vec3 WHITE   {1.0f, 1.0f, 1.0f};
static constexpr vec3 BLACK   {0.0f, 0.0f, 0.0f};


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


struct Transform
{
    vec3  position;
    vec3  rotation;
    float scale;
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*positions), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*uv_coords), (void *) position_size);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*normals),   (void *) (position_size + texture_size));

    return { vao, vertex_count };
}


int main()
{
    Window window = CreateWindow(2880, 1710);

    auto camera = Camera();
    auto shader = CreateShader("Basic", LoadFileToString("../resources/shaders/basic.vs.glsl").get(), LoadFileToString("../resources/shaders/basic.fs.glsl").get());
    auto mesh   = CreateMesh(Cube::VERTICES, Cube::UV_COORDS, Cube::NORMALS, 36);

    entt::registry registry;
    float i = -1.0f;
    for(const auto color : { RED, GREEN, BLUE })
    {
        const auto entity = registry.create();
        registry.emplace<Transform>(entity, vec3{i,0,0}, vec3{0,0,0}, 0.3f);
        registry.emplace<Renderable>(entity, color, &mesh);
        i += 1.0f;
    }

    while (!glfwWindowShouldClose(window.id))
    {
        auto view = ViewMatrix(camera);
        auto projection = ProjectionMatrix(camera);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.id);
        for(auto [entity, transform, renderable]: registry.view<const Transform, const Renderable>().each())
        {
            glBindVertexArray(renderable.mesh->id);
            auto model = glm::mat4();
            model = glm::translate(model, transform.position);
            model = glm::rotate(model, glm::radians(transform.rotation.x), vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.y), vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.z), vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, vec3(transform.scale, transform.scale, transform.scale));

            SetUniform(shader, "model", model);
            SetUniform(shader, "view", view);
            SetUniform(shader, "projection", projection);
            SetUniform(shader, "object_color", renderable.color);
            glDrawArrays(GL_TRIANGLES, 0, renderable.mesh->count);
        }
        glUseProgram(0);

        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    return 0;
}
