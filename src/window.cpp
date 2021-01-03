#include "window.h"

#include "debug.h"



void FrameBufferSizeCallback(GLFWwindow* id, int width, int height)
{
    // For retina displays width and height will end up significantly higher than the original input values.
    // Change the OpenGL render area.
    glViewport(0, 0, width, height);
}



Window CreateWindow(int width, int height)
{
    Window window { nullptr };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#if defined(DEBUG)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif


    window.id = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    ASSERT(window.id != nullptr, "Failed to create GLFW window");

    // 'width' and 'height' will be set in 'FrameBufferSizeCallback' as some displays (like Retina displays) use more
    // 4 pixels for every pixel.
    glfwMakeContextCurrent(window.id);

    ASSERT(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Failed to initialize GLAD");

    glfwSetFramebufferSizeCallback(window.id, FrameBufferSizeCallback);
//    glfwSetKeyCallback(window.id, key_callback);
//    glfwSetCursorPosCallback(window.id, MouseCallback);
//    glfwSetScrollCallback(window.id, ScrollCallback);

    glfwSetInputMode(window.id, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the framebuffer size at startup. The framebuffer doesn't need to match the width and height off the window.
    // On retina screens, the resolution is twice the actual width and height, as each unit is represented by 4 pixels.
    int resolution_width, resolution_height;
    glfwGetFramebufferSize(window.id, &resolution_width, &resolution_height);
    FrameBufferSizeCallback(window.id, resolution_width, resolution_height);

    INFO("Successfully created 'GLFWwindow'");

    return window;
}
