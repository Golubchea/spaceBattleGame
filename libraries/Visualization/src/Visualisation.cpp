#include <Visualization/Model/Entities/Axes.hpp>
#include <Visualization/Model/Shaders/ModelShader.hpp>
#include <Visualization/Visualization.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <Visualization/Model/Entities/Loaders/stb_image.h>

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_GUID(ModelComponent, "ModelComponent")

Visualization::Visualization(std::shared_ptr<Signal> key_signal,
                             std::shared_ptr<World> wrld, int width, int height,
                             const char *title)
    : window(width, height, title), scene_(wrld),
      inputHandler(key_signal, window.getHandle(), camera) {

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK); // Отсекать задние грани
  glFrontFace(GL_CCW); // Фронтальные грани определяются против часовой стрелки
}

Visualization::~Visualization() { glfwTerminate(); }

bool Visualization::isClosed() { return window.shouldClose(); }

void Visualization::run()
{
    setTime();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    inputHandler.processInput(deltaTime);

    scene_.renderAll(camera.getViewMatrix(), camera.getProjection(),
                     camera.getPosition());
    window.pollEvents();
}

void Visualization::setTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Visualization::rotateModel(float azimuth, float elevation)
{
    // model->position = glm::mat4(1.0f);
    // model->position = glm::rotate(model->position,
    //                               glm::radians(azimuth),
    //                               glm::vec3(0.0f, 1.0f, 0.0f));
    // model->position = glm::rotate(model->position,
    //                               glm::radians(elevation),
    //                               glm::vec3(1.0f, 0.0f, 0.0f));
}
