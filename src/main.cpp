#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "window.h"
#include "shader.h"
#include "utils.h"


struct Vertex_
{
    vec3 position;
    vec2 uv_coord;
    vec3 normal;
};


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
struct Input { GLFWwindow* window; };
struct InputBuffer
{
    bool left;
    bool right;
    bool forward;
    bool backward;
    bool up;
    bool down;
    bool rot_left;
    bool rot_right;

    bool operator== (InputBuffer other) const noexcept
    {
        return
            left      == other.left      &&
            right     == other.right     &&
            forward   == other.forward   &&
            backward  == other.backward  &&
            up        == other.up        &&
            down      == other.down      &&
            rot_left  == other.rot_left  &&
            rot_right == other.rot_right;
    }
};
struct Velocity
{
    vec3 data;
    vec3 rot;
};
struct Camera
{
    float near = 0.1f;
    float far  = 100.0f;
    float fov  = 80.0f;
    float aspect_ratio   = 1.0f;
    bool is_orthographic = false;

    [[nodiscard]] mat4 projection(const Camera& camera) const noexcept
    {
        if (camera.is_orthographic)
            return this->orthographic_projection();
        else
            return this->perspective_projection();
    }

    [[nodiscard]] mat4 orthographic_projection() const noexcept
    {
        mat4 projection(1.0f);
        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, this->near, this->far);
        return projection;
    }

    [[nodiscard]] mat4 perspective_projection() const noexcept
    {
        mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(this->fov), this->aspect_ratio, this->near, this->far);
        return projection;
    }
};


mat4 ViewMatrix(const vec3 position, const vec3 forward)
{
    mat4 view(1.0f);
    static constexpr vec3 UP {0.0f, 1.0f, 0.0f};
    view = glm::lookAt(position, position + forward, UP);
    return view;
}

#include <limits>
#include <cstddef>
Mesh CreateMesh(const std::vector<Vertex_>& vertices)
{
    // Create vertex array buffer to store vertex buffers and element buffers.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer to put our data into video memory.
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_), vertices.data(), GL_STATIC_DRAW);

    // Tell OpenGL the data's format.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_), (void *) offsetof(Vertex_, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_), (void *) offsetof(Vertex_, uv_coord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_), (void *) offsetof(Vertex_, normal));

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
    // TODO(ted): Maybe use some of these tips https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices.

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



struct Event
{
    int id;
};

static InputBuffer buffer1;
static InputBuffer buffer2;
static const InputBuffer* read_buffer  = &buffer1;
static       InputBuffer* write_buffer = &buffer2;

void KeyCallback(GLFWwindow* id, int key, int scancode, int action, int mods)
{
    Window* window = (Window*) glfwGetWindowUserPointer(id);

    bool down = action != GLFW_RELEASE;

    InputBuffer* input = write_buffer;

    if (key == GLFW_KEY_A)          input->left      = down;
    if (key == GLFW_KEY_D)          input->right     = down;
    if (key == GLFW_KEY_W)          input->forward   = down;
    if (key == GLFW_KEY_S)          input->backward  = down;
    if (key == GLFW_KEY_SPACE)      input->up        = down;
    if (key == GLFW_KEY_LEFT_SHIFT) input->down      = down;
    if (key == GLFW_KEY_Q)          input->rot_left  = down;
    if (key == GLFW_KEY_E)          input->rot_right = down;


    // TODO(ted): Make atomic!
    auto* temp = const_cast<InputBuffer*>(read_buffer);
    read_buffer  = write_buffer;
    write_buffer = temp;
}



class Clock
{
public:
    double fps;
    int    frames;

    Clock() : fps(0), frames(1), last_time(glfwGetTime()) {}

    double tick()
    {
        double delta = now() - last_time;

        if (delta > 1.0f)
        {
            fps = delta / double(frames);
            frames = 0;
            last_time = now();
        }
        ++frames;  // Important! Frames must always be greater than 0.

        return delta;
    }

    static double now()
    {
        return glfwGetTime();
    }

private:
    double last_time;
};



void UpdateVelocity(entt::registry& registry, double delta)
{
    auto dt = float(delta);  // TODO(ted): Seems buggy...

    registry.view<Transform, Velocity, const Input>().each(
        [dt](auto& transform, auto& velocity, auto& input)
        {
            float speed     = 0.05f;
            float rot_speed = 0.05f;

            if (glfwGetKey(input.window, GLFW_KEY_A) == GLFW_PRESS)
                velocity.data.x = -speed;
            else if (glfwGetKey(input.window, GLFW_KEY_D) == GLFW_PRESS)
                velocity.data.x =  speed;
            else
                velocity.data.x = 0;

            if (glfwGetKey(input.window, GLFW_KEY_W) == GLFW_PRESS)
                velocity.data.z = -speed;
            else if (glfwGetKey(input.window, GLFW_KEY_S) == GLFW_PRESS)
                velocity.data.z =  speed;
            else
                velocity.data.z = 0;

            if (glfwGetKey(input.window, GLFW_KEY_SPACE) == GLFW_PRESS)
                velocity.data.y =  speed;
            else if (glfwGetKey(input.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                velocity.data.y = -speed;
            else
                velocity.data.y = 0;

            if (glfwGetKey(input.window, GLFW_KEY_Q) == GLFW_PRESS)
                velocity.rot.y  =  rot_speed;
            else if (glfwGetKey(input.window, GLFW_KEY_E) == GLFW_PRESS)
                velocity.rot.y  = -rot_speed;
            else
                velocity.rot.y  = 0;
        }
    );

    registry.view<Transform, const Velocity>().each(
        [dt](auto& transform, const auto& velocity)
        {
            transform.position += velocity.data;
            transform.rotation += velocity.rot;
        }
    );
}

std::tuple<mat4, mat4> UpdateCamera(entt::registry& registry, entt::entity camera)
{
    auto transform = registry.get<Transform>(camera);
    auto view = ViewMatrix(transform.position, transform.rotation);

    auto data = registry.get<Camera>(camera);
    auto projection = data.perspective_projection();

    return {view, projection};
}

void Render(entt::registry& registry, const Shader& shader, entt::entity camera)
{
    auto [view, projection] = UpdateCamera(registry, camera);

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
}


Mesh LoadAsset(const std::string& inputfile)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;
    Assert(reader.ParseFromFile(inputfile, reader_config), "");

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<Vertex_> vertices;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                vertices.push_back({
                       {vx, vy, vz}, {tx, ty}, {nx, ny, nz}
               });
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }

    auto mesh = CreateMesh(vertices);
    return mesh;
}


int main()
{
    entt::registry registry;
    Window window = CreateWindow(2880, 1710, &registry, KeyCallback);
    glfwSetWindowUserPointer(window.id, &window);

    auto wreath = LoadAsset("../resources/models/kenney_holidaykit_2/Models/OBJ format/wreath.obj");
    auto post   = LoadAsset("../resources/models/kenney_holidaykit_2/Models/OBJ format/lightpost.obj");
    auto tree   = LoadAsset("../resources/models/kenney_holidaykit_2/Models/OBJ format/treeDecorated.obj");
    auto meshes = std::array{ wreath, post, tree };

    auto shader = CreateShader("Basic", LoadFileToString("../resources/shaders/basic.vs.glsl").get(), LoadFileToString("../resources/shaders/basic.fs.glsl").get());
//    auto mesh   = CreateMesh(Cube::VERTICES, Cube::UV_COORDS, Cube::NORMALS, 36);

    float x = -1.0f;
    auto colors = std::array{ RED, GREEN, BLUE };
    for (int i = 0; i < 3; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<Transform>(entity, vec3{x,0,0}, vec3{0,0,0}, 0.3f);
        registry.emplace<Renderable>(entity, colors[i], &meshes[i]);
        x += 1.0f;
    }

    const auto camera = registry.create();
    registry.emplace<Transform>(camera, vec3{0, 1, 3}, vec3(0.0f, 0.0f, -1.0f), 1.0f);
    registry.emplace<Velocity>(camera, vec3{0, 0, 0});
    registry.emplace<Input>(camera, window.id);
    registry.emplace<Camera>(camera);


    Clock clock;
    while (!glfwWindowShouldClose(window.id))
    {
        double delta = clock.tick();

        UpdateVelocity(registry, delta);
        Render(registry, shader, camera);

        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    return 0;
}
