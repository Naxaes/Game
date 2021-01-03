#include <tuple>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>


// ---- CALLBACKS ----
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, double x_position, double y_position);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);


struct Window
{
    GLFWwindow* id = nullptr;  // Should never be released.
};
Window CreateWindow(int width, int height);
