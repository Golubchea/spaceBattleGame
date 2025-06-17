// IComponentProvider.hpp
#pragma once
#include "ECSEngineLib/Components/PositionComponent.hpp"
#include "ECSEngineLib/Components/RotationComponent.hpp"
#include "ECSEngineLib/Components/VelocityComponent.hpp"
#include "ECSEngineLib/Entity.hpp"
#include "Model/Entities/modelComponent.h"
#include <memory>

class IComponentProvider {
public:
  virtual ~IComponentProvider() = default;
  virtual std::vector<uint64_t> getAllEnties() = 0;
  virtual std::shared_ptr<PositionComponent> GetPosition(Entity entity) = 0;
  virtual std::shared_ptr<VelocityComponent> GetVelocity(Entity entity) = 0;
  virtual std::shared_ptr<RotationComponent> GetRotation(Entity entity) = 0;
  virtual std::shared_ptr<ModelComponent> GetModel(Entity entity) = 0;
};
