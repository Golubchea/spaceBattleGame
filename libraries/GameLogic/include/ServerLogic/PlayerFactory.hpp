#pragma once
#include "Player.hpp"
#include <ECSEngineLib/World.hpp>

#include <ECSEngineLib/Components/FuelComponent.hpp>
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/VelocityComponent.hpp>
#include <InversionOfControl/InversionOfControl.h>
#include <Visualization/Model/Entities/EntityFactory.hpp>
#include <Visualization/Model/Entities/modelComponent.h>
#include <Visualization/Model/Entities/shaderComponent.h>

class PlayerFactory {
public:
    virtual ~PlayerFactory() = default;
    virtual std::shared_ptr<Player> CreatePlayer(glm::vec3 pos,
                                                 glm::vec3 velocity,
                                                 float angle, float fuel,
                                                 int missles) = 0;

    virtual std::shared_ptr<Player> CreateAxis() = 0;
    virtual std::shared_ptr<Player> CreateGrid() = 0;
};

class DefaultPlayerFactory : public PlayerFactory {
public:
  explicit DefaultPlayerFactory(const shared_ptr<World> &world)
      : world_(world) {}

  std::shared_ptr<Player> CreatePlayer(glm::vec3 pos, glm::vec3 velocity,
                                       float angle, float fuel,
                                       int missles) override {
    auto entity = world_->CreateEntity();

    // Добавляем начальные компоненты
    world_->AddComponent<PositionComponent>(
        entity, std::array<double, 2>{pos.x, pos.z});
    world_->AddComponent<VelocityComponent>(
        entity, std::array<double, 2>{velocity.x, velocity.z});
    world_->AddComponent<FuelComponent>(entity, fuel);

    world_->AddComponent<ModelComponent>(entity);
    auto obj = world_->GetComponent<ModelComponent>(entity);

    obj->setModel(std::move(EntityFactory::createEntity("Model", pos, angle)));
    return std::make_shared<Player>(entity, world_);
  }

    std::shared_ptr<Player> CreateAxis() override {
      auto entity = world_->CreateEntity();
      // Добавляем начальные компоненты
      world_->AddComponent<PositionComponent>(entity,
                                              std::array<double, 2>{0, 0});

      world_->AddComponent<ModelComponent>(entity);
      auto obj = world_->GetComponent<ModelComponent>(entity);
      auto model = EntityFactory::createEntity("Axis");
      obj->setModel(model);
      return std::make_shared<Player>(entity, world_);
    }

    std::shared_ptr<Player> CreateGrid() override {
      auto entity = world_->CreateEntity();

      // Добавляем начальные компоненты
      world_->AddComponent<PositionComponent>(entity,
                                              std::array<double, 2>{0, 0});

      // world_.AddComponent<ModelComponent>(entity, std::move(model));
      world_->AddComponent<ModelComponent>(entity);
      auto obj = world_->GetComponent<ModelComponent>(entity);
      auto model = EntityFactory::createEntity("Grid");
      obj->setModel(model);
      return std::make_shared<Player>(entity, world_);
    }

private:
  shared_ptr<World> world_;
};
