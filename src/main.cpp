#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

#include "window.h"
#include "shader.h"
#include "utils.h"
#include "loader.h"
#include "model.h"


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


static constexpr vec3 WORLD_AXIS_RIGHT   = vec3(1.0f,  0.0f,  0.0f);
static constexpr vec3 WORLD_AXIS_UP      = vec3(0.0f,  1.0f,  0.0f);
static constexpr vec3 WORLD_AXIS_FORWARD = vec3(0.0f,  0.0f, -1.0f);


struct Transform
{
    vec3  position;
    vec3  rotation;
    float scale;
};
struct Renderable
{
     vec3     color;
     Mesh*    mesh;
     Texture* texture;
};
struct HitBox
{
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;
};
struct Physics
{
    float  gravity;
    HitBox hitbox;
    bool   is_static;
};
struct Input
{
    GLFWwindow* window;
};
struct Velocity
{
    vec3 data;
    vec3 rot;
};
struct Camera
{
    vec3 position;

    vec3 forward  = WORLD_AXIS_FORWARD;
    vec3 up       = WORLD_AXIS_UP;
    vec3 right    = WORLD_AXIS_RIGHT;

    double pitch  = 0.0f;
    double yaw    = 0.0f;

    float speed = 4.0f;

    float near = 0.1f;
    float far  = 1000.0f;
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


bool Intersect(const vec3& pos_a, const HitBox& hitbox_a, const vec3& pos_b, const HitBox& hitbox_b)
{
    bool a = hitbox_a.min_x + pos_a.x <= hitbox_b.max_x + pos_b.x;
    bool b = hitbox_a.max_x + pos_a.x >= hitbox_b.min_x + pos_b.x;

    bool c = hitbox_a.min_y + pos_a.y <= hitbox_b.max_y + pos_b.y;
    bool d = hitbox_a.max_y + pos_a.y >= hitbox_b.min_y + pos_b.y;

    bool e = hitbox_a.min_z + pos_a.z <= hitbox_b.max_z + pos_b.z;
    bool f = hitbox_a.max_z + pos_a.z >= hitbox_b.min_z + pos_b.z;

    return a && b && c && d && e && f;
}


void Update(entt::registry& registry)
{
    static float last_time = static_cast<float>(glfwGetTime());

    float time = static_cast<float>(glfwGetTime());
    float dt   = time - last_time;
    last_time  = time;

    registry.view<Camera, const Input>().each(
        [dt](auto& camera, const auto& input)
        {
            static bool first_call = true;

            vec3  local  = vec3();
            float speed  = camera.speed * dt;
            auto  window = input.window;

//            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                static double last_x = 400;
                static double last_y = 300;
                static double sensitivity = 0.2f;

                if (first_call)
                {
                    last_x = x;
                    last_y = y;
                    first_call = false;
                }

                double x_offset = glm::radians((x - last_x) * sensitivity);
                double y_offset = glm::radians((last_y - y) * sensitivity);  // Reversed since y-coordinates range from bottom to top.

                last_x = x;
                last_y = y;

                Rotate(camera, vec2(x_offset, y_offset));
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                speed *= 2.0f;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                local.z = speed;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                local.z = -speed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                local.x = speed;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                local.x = -speed;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                local.y = speed;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                local.y = -speed;

            Move(camera, local);
        }
    );

    registry.view<Velocity, const Physics>().each(
        [](auto& velocity, const auto& physics)
        {
            velocity.data.y -= physics.gravity;
        }
    );

    registry.view<Transform, const Velocity>().each(
        [](auto& transform, const auto& velocity)
        {
            transform.position += velocity.data;
            transform.rotation += velocity.rot;
        }
    );

    registry.view<Transform, const Velocity>().each(
            [](auto& transform, const auto& velocity)
            {
                transform.position += velocity.data;
                transform.rotation += velocity.rot;
            }
    );

    registry.view<const Transform, Physics, Velocity>().each(
        [&registry](const auto entity_a, const auto& transform_a, auto& physics_a, auto& velocity_a)
        {
            if (physics_a.is_static)
                return;

            for (auto [entity_b, transform_b, physics_b] : registry.view<const Transform, const Physics>().each())
            {
                if (entity_a == entity_b)
                    continue;

                if (Intersect(transform_a.position, physics_a.hitbox, transform_b.position, physics_b.hitbox))
                {
                    if (physics_b.is_static)
                    {
                        physics_a.gravity = 0;
                        velocity_a.data   = vec3(0);
                    }
                }
            }
        }
    );
}
void Move(Camera& camera, vec3 local_direction)
{
    camera.position += camera.right   * local_direction.x;
    camera.position += camera.up      * local_direction.y;
    camera.position += camera.forward * local_direction.z;
}
// Does not support roll. Expects radians.
void Rotate(Camera& camera, vec2 rotation)
{
    camera.yaw   = std::fmod(camera.yaw + rotation.x, 2 * M_PI);
    camera.pitch = glm::clamp(camera.pitch + rotation.y, -M_PI_2 + 0.001f, M_PI_2 - 0.001f);

    camera.forward.x =  sin(camera.yaw) * cos(camera.pitch);
    camera.forward.z = -cos(camera.yaw) * cos(camera.pitch);  // Invert as we treat (0, 0, -1) as forward.
    camera.forward.y =  sin(camera.pitch);

    camera.forward = glm::normalize(camera.forward);
    camera.right   = glm::normalize(glm::cross(WORLD_AXIS_UP,  -camera.forward));
    camera.up      = glm::normalize(glm::cross(-camera.forward, camera.right));
}
std::tuple<mat4, mat4> UpdateCamera(entt::registry& registry, entt::entity camera)
{
    auto data = registry.get<Camera>(camera);
    auto view = ViewMatrix(data.position, data.forward);
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
    auto model = glm::mat4();
    model = glm::translate(model, vec3(0));
//    model = glm::rotate(model, glm::radians(transform.rotation.x), vec3(1.0f, 0.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(transform.rotation.y), vec3(0.0f, 1.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(transform.rotation.z), vec3(0.0f, 0.0f, 1.0f));
//    model = glm::scale(model, vec3(transform.scale, transform.scale, transform.scale));
    SetUniform(shader, "model", model);
    SetUniform(shader, "view",  view);
    SetUniform(shader, "projection", projection);
    GLuint bound_mesh_id = -1;
    for (auto [entity, transform, renderable]: registry.view<const Transform, const Renderable>().each())
    {
        if (bound_mesh_id != renderable.mesh->id)
        {
            glBindVertexArray(renderable.mesh->id);
            bound_mesh_id = renderable.mesh->id;
        }
        SetTexture2D(shader, "diffuse", 0, renderable.mesh->texture);
//        SetUniform(shader,   "object_color", renderable.color);
        glDrawArrays(GL_TRIANGLES, 0, renderable.mesh->count);
    }
    glUseProgram(0);
}


int main()
{
    entt::registry registry;
    Window window = CreateWindow(2880, 1710, "Game");
    glfwSetWindowUserPointer(window.id, &registry);

    const auto empty_texture = CreateTexture2D(Image::empty());

//    auto [all_meshes, all_materials] = LoadScene("../resources/models/sponza/sponza.obj", "../resources/models/sponza/");
    auto [all_meshes, all_materials] = LoadScene("../resources/models/cube.obj", "../resources/models/");
    std::vector<Mesh> meshes;
    for (const auto& data : all_meshes)
    {
        auto mesh = CreateMesh(data.vertices);
        if (data.material)
        {
            auto material = data.material;

            if (material->diffuse_map)
                mesh.texture = CreateTexture2D(material->diffuse_map.value());
            else
                mesh.texture = empty_texture;   // TODO(ted): Colored material.
        }
        else
        {
            mesh.texture = empty_texture;
        }

        meshes.push_back(mesh);
    }

    auto shader = CreateShader("Basic", LoadFileToString("../resources/shaders/basic.vs.glsl").get(), LoadFileToString("../resources/shaders/basic.fs.glsl").get());

    float x = -1.0f;
    int   i = 0;
    auto colors = std::array{ RED, GREEN, BLUE };
    for (std::size_t i = 0; i < meshes.size(); ++i)
    {
        auto& mesh = meshes[i];
        auto& data = all_meshes[i];

        const auto entity = registry.create();
        registry.emplace<Transform>(entity, vec3{x*data.mesh_id,2.0f,0}, vec3{0,0,0}, 0.3f);
        registry.emplace<Renderable>(entity, colors[data.material_id % 3], &mesh);
//        registry.emplace<Velocity>(entity, vec3{0, 0, 0}, vec3{0, 0, 0});
//        registry.emplace<Physics>(entity, 0.005f, HitBox{-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0}, false);
        x += 1.0f;
    }


    registry.sort<Renderable>([](const auto& lhs, const auto& rhs) {
        return lhs.mesh->id < rhs.mesh->id;
    });


//    const auto floor = registry.create();
//    registry.emplace<Transform>(floor, vec3{0,0,0}, vec3{0,0,0}, 0.3f);
//    registry.emplace<Physics>(floor, 0.0f, HitBox{-10.0f, 10.0f, -10.0f, 0.0f, -10.0f, 10.0}, true);


    const auto camera = registry.create();
    registry.emplace<Input>(camera, window.id);
    registry.emplace<Camera>(camera, vec3{0, 2.0f, 3.0f});


    while (!glfwWindowShouldClose(window.id))
    {
        Update(registry);
        Render(registry, shader, camera);

        glfwSwapBuffers(window.id);
        glfwPollEvents();
    }

    return 0;
}
