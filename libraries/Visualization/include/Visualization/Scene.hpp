#pragma once
#include "ECSEngineLib/Components/RotationComponent.hpp"
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
  // void renderAll(const glm::mat4 &view, const glm::mat4 &projection,
  //                const glm::vec3 &cameraPos) {

  //   if (!world_->components().empty()) {

  //     auto positionView =
  //         world_->components()[typeid(PositionComponent).name()];

  //     auto velocityView =
  //         world_->components()[typeid(VelocityComponent).name()];

  //     auto angleView =
  //     world_->components()[typeid(RotationComponent).name()];

  //     // Vector<double, 2> newPosition = adapter.getPosition() +
  //     // adapter.getVelocity();

  //     auto modelView = world_->components()[typeid(ModelComponent).name()];

  //     // // Перебираем все Entity, которые есть в обоих
  //     for (const auto &[entity, posComponent] : positionView) {
  //       auto modelIt = modelView.find(entity);
  //       if (modelIt == modelView.end())
  //         continue;
  //       auto velIt = modelView.find(entity);
  //       // if (modelIt == modelView.end())
  //       //   continue;
  //       auto angleIt = angleView.find(entity);
  //       // if (modelIt == angleView.end())
  //       //   continue;

  //       auto *position = dynamic_cast<PositionComponent
  //       *>(posComponent.get()); auto *velocity =
  //       dynamic_cast<VelocityComponent *>(velIt->second.get()); auto
  //       *modelComp = dynamic_cast<ModelComponent *>(modelIt->second.get());

  //       auto *rotationComp =
  //           dynamic_cast<RotationComponent *>(angleIt->second.get());

  //       if (!position || !modelComp || !modelComp->getModel())
  //         continue;

  //       // Обновляем матрицу модели из позиции
  //       glm::vec3 pos(position->getPosition()[0], 0.0f,
  //                     position->getPosition()[1]);
  //       glm::vec3 vel = {0, 0, 0};
  //       if (velocity) {
  //         vel = {velocity->getVelocity()[0], 0.0f,
  //         velocity->getVelocity()[1]};
  //       }
  //       glm::mat4 modelMatrix = glm::mat4(1.0f);

  //       if (rotationComp) {
  //         modelMatrix = glm::rotate(modelMatrix, rotationComp->angle(),
  //                                   glm::vec3(0.0f, 1.0f, 0.0f));
  //       }
  //       modelMatrix = glm::translate(modelMatrix, pos + vel);

  //       // Устанавливаем матрицу в модель
  //       modelComp->getModel()->setPosition(modelMatrix);
  //       modelComp->getModel()->renderEntity(view, projection, cameraPos);
  //     }
  //   }
  // }

  void renderAll(const glm::mat4 &view, const glm::mat4 &projection,
                 const glm::vec3 &cameraPos) {

    if (world_->components().empty())
      return;

    auto components = world_->components();

    // Получаем все нужные компоненты
    auto positionView = components[typeid(PositionComponent).name()];
    auto velocityView = components[typeid(VelocityComponent).name()];
    auto angleView = components[typeid(RotationComponent).name()];
    auto modelView = components[typeid(ModelComponent).name()];

    for (const auto &[entity, posComponent] : positionView) {
      // Проверяем, есть ли у этой сущности модель
      auto modelIt = modelView.find(entity);
      if (modelIt == modelView.end())
        continue;

      // Получаем позицию
      auto *position = dynamic_cast<PositionComponent *>(posComponent.get());
      if (!position)
        continue;

      // Получаем скорость
      auto velIt = velocityView.find(entity);
      VelocityComponent *velocity = nullptr;
      if (velIt != velocityView.end()) {
        velocity = dynamic_cast<VelocityComponent *>(velIt->second.get());
      }

      // Получаем угол поворота
      RotationComponent *rotationComp = nullptr;
      auto angleIt = angleView.find(entity);
      if (angleIt != angleView.end()) {
        rotationComp = dynamic_cast<RotationComponent *>(angleIt->second.get());
      }

      // Получаем модель
      auto *modelComp = dynamic_cast<ModelComponent *>(modelIt->second.get());
      if (!modelComp || !modelComp->getModel())
        continue;

      // Строим матрицу модели
      glm::vec3 pos(position->getPosition()[0], 0.0f,
                    position->getPosition()[1]);
      glm::vec3 vel(0.0f);

      if (velocity) {
        vel = {velocity->getVelocity()[0], 0.0f, velocity->getVelocity()[1]};
      }

      glm::mat4 modelMatrix = glm::mat4(1.0f);

      modelMatrix = glm::translate(modelMatrix, pos + vel);
      if (rotationComp) {
        modelMatrix = glm::rotate(modelMatrix, rotationComp->angle(),
                                  glm::vec3(0.0f, 1.0f, 0.0f));
      }

      // Рендерим
      modelComp->getModel()->setPosition(modelMatrix);
      modelComp->getModel()->renderEntity(view, projection, cameraPos);
    }
  }

  // Очистка сцены
  void clear() { world_->clear(); }

  ~Scene() { clear(); }
};
