#pragma once

#include <glad/glad.h>

#include "Camera.hpp"
#include "InputHandler.hpp"
#include "Window.hpp"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include <Signals/Signals.hpp>
#include <Visualization/Model/Entities/model.h>
#include <Visualization/Model/Shaders/Shader.hpp>
#include <Visualization/Scene.hpp>

class CoordinateSystem;

class Visualization
{
public:
  Visualization(std::shared_ptr<Signal> key_signal, std::shared_ptr<World> wrld,
                int width = 800, int height = 600, const char *title = "Game");

  void rotateModel(float azimuth, float elevation);

  void addPlayer(glm::vec3 pos = {0.0f, 1.0f, 0.0f},
                 float angle = glm::radians(45.0f), uint64_t id = 0);

  ~Visualization();
  bool isClosed();

  void run();

private:
    void setTime();

    Window window;
    Camera camera;
    InputHandler inputHandler;
    Scene scene_;

    // todo: add to scene
    // std::unique_ptr<Model> model;
    // std::unique_ptr<Shader> shader;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};
