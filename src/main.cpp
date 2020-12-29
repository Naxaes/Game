#include <iostream>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "window.h"


using glm::vec2;
using glm::vec3;


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

    static constexpr vec2 UV_COORDINATES[] = {
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
struct Velocity { float x, y, z; };


void update(entt::registry &registry) {
    auto view = registry.view<const Position, Velocity>();

    std::cout << "--- Using callback ---" << std::endl;
    view.each([](const auto &position, auto &velocity) {
        std::cout << "Position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Velocity (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    });

    std::cout << "--- Using extended callback ---" << std::endl;
    view.each([](const auto entity, const auto &position, auto &velocity) {
        std::cout << "Entity " << static_cast<std::uint32_t>(entity) << std::endl;
        std::cout << "Position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Velocity (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    });
}


int main()
{
//    Window window = CreateWindow(2880, 1710);

    entt::registry registry;

    for(auto i = 0u; i < 10u; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<Position>(entity, i * 1.f, i * 1.f, i * 1.f);
        if (i % 2 == 0)
            registry.emplace<Velocity>(entity, i * .1f, i * .1f, i * .1f);
    }

    update(registry);
    return 0;
//
//    while (!glfwWindowShouldClose(window.id))
//    {
//        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glfwSwapBuffers(window.id);
//        glfwPollEvents();
//    }
//
//    return 0;
}
