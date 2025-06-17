#pragma once

#include <GLFW/glfw3.h>
#include <stdexcept>

class Window
{
public:
    Window(int width, int height, const char *title)
        : width(width)
        , height(height)
        , title(title)
    {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        glfwWindowHint(GLFW_SAMPLES, 4); // 2 мультисемплинг

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    ~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool shouldClose() const { return glfwWindowShouldClose(window); }

    void pollEvents()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    GLFWwindow *getHandle() const { return window; }

    int getHeight() const;

    int getWidth() const;

private:
    GLFWwindow *window;
    int width, height;
    const char *title;

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
};

inline int Window::getHeight() const
{
    return height;
}

inline int Window::getWidth() const
{
    return width;
}
