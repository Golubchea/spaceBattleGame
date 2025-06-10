#pragma once
#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <ECSEngineLib/World.hpp>
#include <ServerLogic/CommandExecutor.hpp>
#include <ServerLogic/Commands/DestroyEntityCommand.hpp>
#include <memory>
#include <unordered_map>

#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <ECSEngineLib/World.hpp>
#include <ServerLogic/CommandExecutor.hpp>

class CollisionSystem {
public:
  explicit CollisionSystem(std::shared_ptr<World> world,
                           std::shared_ptr<DynamicBVH> bvh,
                           std::shared_ptr<CommandExecutor> command_executor);

  void onWorldUpdate(float dt);

private:
  std::shared_ptr<World> world_;
  std::shared_ptr<DynamicBVH> bvh_;
  std::shared_ptr<CommandExecutor> command_executor_;

  // Храним предыдущие позиции для определения движения
  std::unordered_map<Entity, Vec2> last_positions_;
};
