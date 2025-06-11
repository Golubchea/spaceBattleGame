#include "ECSEngineLib/Components/PositionComponent.hpp"
#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/RotationComponent.hpp>
#include <ServerLogic/Commands/DestroyEntityCommand.hpp>
#include <ServerLogic/Systems/CollisionSystem.hpp>
#include <unordered_set>

namespace {

float sizeX = 0.5;
float sizeY = 0.5;

} // namespace

CollisionSystem::CollisionSystem(
    std::shared_ptr<World> world, std::shared_ptr<DynamicBVH> bvh,
    std::shared_ptr<CommandExecutor> command_executor)
    : world_(std::move(world)), bvh_(std::move(bvh)),
      command_executor_(std::move(command_executor)) {

  // Подписываемся на обновление мира
  world_->RegisterSystem([this](float dt) { this->onWorldUpdate(dt); });
}

void CollisionSystem::onWorldUpdate(float dt) {
  if (!world_)
    return;

  // === Шаг 1: Обновляем только те объекты, у которых изменилась позиция ===

  auto entities = world_->getAllEntitiesWith<RotationComponent>();
  size_t i = 0;
  for (const auto &entity : entities) {
    auto pos = world_->GetComponent<PositionComponent>(entity);
    if (!pos)
      continue;

    float angle = 0.0f;

    // Проверяем, есть ли у объекта RotationComponent
    if (auto rot = world_->GetComponent<RotationComponent>(entity)) {
      angle = rot->angle();
    }

    bvh_->objects().resize(entities.size());

    Rect rect(Vec2(pos->getPosition()[0], pos->getPosition()[1]), sizeX, sizeY,
              angle);
    bvh_->objects().at(i) = {rect, entity};
  }
  bvh_->build();

  // === Шаг 2: Ищем коллизии ===
  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh_->findCollisions(collisions);

  for (const auto &[a, b] : collisions) {
    auto destroyCmd = std::make_shared<DestroyEntityCommand>(a, world_);
    command_executor_->EnqueueCommand(destroyCmd, nullptr);
    command_executor_->WorkerLoop();
  }

  bvh_->objects().clear();
}
