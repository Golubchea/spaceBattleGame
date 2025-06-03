#pragma once
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/VelocityComponent.hpp>
#include <ECSEngineLib/World.hpp>
#include <Visualization/Model/Entities/Axes.hpp>
#include <Visualization/Model/Entities/Grid.hpp>
#include <Visualization/Model/Entities/model.h>
#include <Visualization/Model/Entities/modelComponent.h>
#include <Visualization/Model/Shaders/SimpleShader.hpp>
#include <Visualization/Utils/UUID.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>

// Класс для управления сценой
class Scene
{
private:
  std::shared_ptr<World> world_ = nullptr;

public:
  Scene(std::shared_ptr<World> wrld) : world_(wrld) {}

  // Обновление всех сущностей
  void renderAll(const glm::mat4 &view, const glm::mat4 &projection,
                 const glm::vec3 &cameraPos) {

    if (!world_->components().empty()) {

      auto positionView =
          world_->components()[typeid(PositionComponent).name()];

      auto velocityView =
          world_->components()[typeid(PositionComponent).name()];

      // Vector<double, 2> newPosition = adapter.getPosition() +
      // adapter.getVelocity();

      auto modelView = world_->components()[typeid(ModelComponent).name()];

      // // Перебираем все Entity, которые есть в обоих
      for (const auto &[entity, posComponent] : positionView) {
        auto modelIt = modelView.find(entity);
        if (modelIt == modelView.end())
          continue;
        auto velIt = modelView.find(entity);
        if (modelIt == modelView.end())
          continue;

        auto *position = dynamic_cast<PositionComponent *>(posComponent.get());
        auto *velocity = dynamic_cast<VelocityComponent *>(velIt->second.get());
        auto *modelComp = dynamic_cast<ModelComponent *>(modelIt->second.get());

        if (!position || !modelComp || !modelComp->getModel())
          continue;

        // Обновляем матрицу модели из позиции
        glm::vec3 pos(position->getPosition()[0], 0.0f,
                      position->getPosition()[1]);
        glm::vec3 vel = {0, 0, 0};
        if (velocity) {
          vel = {velocity->getVelocity()[0], 0.0f, velocity->getVelocity()[1]};
        }

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), pos + vel);

        // Устанавливаем матрицу в модель
        modelComp->getModel()->setPosition(modelMatrix);
        modelComp->getModel()->renderEntity(view, projection, cameraPos);
      }
    }
  }

    // Очистка сцены
  void clear() { world_->clear(); }

  ~Scene() { clear(); }
};
