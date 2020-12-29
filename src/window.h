#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>



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

extern double PRESS_TIME_OUT_IN_SECONDS;

template <int action, int key>
bool Pressed(GLFWwindow* window)
{
    static double last = glfwGetTime();
    double time = glfwGetTime();

    if (glfwGetKey(window, key) == action && time - last > PRESS_TIME_OUT_IN_SECONDS)
    {
        last = time;
        return true;
    }
    else
    {
        return false;
    }
}

