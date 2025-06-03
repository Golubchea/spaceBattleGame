#pragma once

#include <GLFW/glfw3.h>
#include <Signals/Signals.hpp>
#include <Visualization/Camera.hpp>
#include <iostream>
#include <memory>
#include <thread>

class InputHandler
{
public:
  InputHandler(std::shared_ptr<Signal> key_signals, GLFWwindow *window,
               Camera &camera)
      : key_signals_(key_signals), window(window), camera(camera), lastX(0.0),
        lastY(0.0), isDragging(false) {
    // Устанавливаем указатель на текущий экземпляр InputHandler для окна
    glfwSetWindowUserPointer(window, this);

    // Устанавливаем обратные вызовы
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
  }

    ~InputHandler()
    {
   
    }

    void processInput(float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            shifted = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            shifted = false;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_W, deltaTime, shifted);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_S, deltaTime, shifted);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(GLFW_KEY_A, deltaTime, shifted);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(GLFW_KEY_D, deltaTime, shifted);
        }

        // Обработка событий
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
          key_signals_->emit("up");
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
          key_signals_->emit("down");
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
          key_signals_->emit("left");
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
          key_signals_->emit("right");
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
          key_signals_->emit("space");
        }
    }

    Camera &getCamera() const;

private:
  // struct sp_port *port = nullptr;

  GLFWwindow *window;
  Camera &camera;
  std::shared_ptr<Signal> key_signals_ = nullptr;
  double lastX, lastY;
  bool isDragging;
  bool shifted = false;

  static void window_size_callback(GLFWwindow *window, int width, int height) {
    InputHandler *handler =
        static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
    if (!handler)
      return; // Защита от nullptr
    handler->getCamera().updateProjection(width, height);
    }

    // Статический метод для обработки движения мыши
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    {
        // Получаем указатель на экземпляр InputHandler
        InputHandler *handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        if (!handler)
            return; // Защита от nullptr

        if (handler->isDragging) {
            float deltaX = xpos - handler->lastX;
            float deltaY = handler->lastY - ypos;
            handler->camera.processMouseMovement(deltaX, deltaY);
        }
        handler->lastX = xpos;
        handler->lastY = ypos;
    }

    // Статический метод для обработки нажатия кнопок мыши
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        // Получаем указатель на экземпляр InputHandler
        InputHandler *handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        if (!handler)
            return; // Защита от nullptr

        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
            handler->isDragging = true;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
            handler->isDragging = false;
        }
    }
};

inline Camera &InputHandler::getCamera() const
{
    return camera;
}
