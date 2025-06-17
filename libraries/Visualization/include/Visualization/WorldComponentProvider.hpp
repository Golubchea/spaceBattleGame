// WorldComponentProvider.hpp
#pragma once
#include "IComponentProvider.hpp"
#include <ECSEngineLib/World.hpp>

class WorldComponentProvider : public IComponentProvider {
public:
  explicit WorldComponentProvider(std::shared_ptr<World> world)
      : world_(world) {}

  std::shared_ptr<PositionComponent> GetPosition(Entity entity) override {
    return world_->GetComponent<PositionComponent>(entity);
  }

  std::vector<uint64_t> getAllEnties() override {
    return world_->getAllEntities();
  }

  std::shared_ptr<VelocityComponent> GetVelocity(Entity entity) override {
    return world_->GetComponent<VelocityComponent>(entity);
  }

  std::shared_ptr<RotationComponent> GetRotation(Entity entity) override {
    return world_->GetComponent<RotationComponent>(entity);
  }

  std::shared_ptr<ModelComponent> GetModel(Entity entity) override {
    return world_->GetComponent<ModelComponent>(entity);
  }

private:
  std::shared_ptr<World> world_;
};
