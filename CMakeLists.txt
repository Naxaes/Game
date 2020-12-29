cmake_minimum_required(VERSION 3.17)
project(Game)

set(CMAKE_CXX_STANDARD 17)


# GLAD
add_library(glad libraries/glad/src/glad.c)
target_include_directories(glad PRIVATE libraries/glad/include/)

# GLFW
add_subdirectory(libraries/glfw-3.3.1)


set(
    SOURCES  # EXCLUDING MAIN!
    src/window.cpp src/debug.cpp src/shader.cpp src/utils.cpp src/camera.cpp
)
add_executable(Game src/main.cpp ${SOURCES})
target_include_directories(Game PRIVATE src/)
target_include_directories(Game PRIVATE libraries/glad/include/)
target_include_directories(Game PRIVATE libraries/glm/)
target_include_directories(Game PRIVATE libraries/entt/src/)
target_link_libraries(Game glad glfw)