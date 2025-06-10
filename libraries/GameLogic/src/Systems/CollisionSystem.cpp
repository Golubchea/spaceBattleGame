#include "ECSEngineLib/Components/PositionComponent.hpp"
#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/RotationComponent.hpp>
#include <ServerLogic/Commands/DestroyEntityCommand.hpp>
#include <ServerLogic/Systems/CollisionSystem.hpp>
#include <iostream>

namespace {

float sizeX = 12.5;
float sizeY = 10.5;

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
  auto entities = world_->getAllEntitiesWith<PositionComponent>();
  for (const auto &entity : entities) {
    auto pos = world_->GetComponent<PositionComponent>(entity);
    if (!pos)
      continue;

    float angle = 0.0f;

    // Проверяем, есть ли у объекта RotationComponent
    if (auto rot = world_->GetComponent<RotationComponent>(entity)) {
      angle = rot->angle();
    }

    Rect rect(Vec2(pos->getPosition()[0], pos->getPosition()[1]), sizeX, sizeY,
              angle);
    bvh_->objects().push_back({rect, entity});

    // bvh_->update(
    //     entity); // предположим, что DynamicBVH поддерживает update(Entity)

    // Проверяем, была ли позиция изменена
    //  auto it = last_positions_.find(entity);
    //  if (it == last_positions_.end() || it->second != current_pos) {
    //    // Нужно обновить объект в BVH по его ID

    //    last_positions_[entity] = current_pos;
    //  }
  }
  bvh_->build();

  // === Шаг 2: Ищем коллизии ===
  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh_->findCollisions(collisions);

  // std::cout << "CollisionSystem::onWorldUpdate(float dt)" << "findCollisions"
  //         << collisions.size();

  // === Шаг 3: Обрабатываем коллизии ===
  for (const auto &[a, b] : collisions) {
    // std::cout << "[Collision] Entity " << a << " and Entity " << b << "\n";

    // Пример: уничтожаем один из объектов
    auto destroyCmd = std::make_shared<DestroyEntityCommand>(b, world_);
    if (command_executor_) {
      command_executor_->EnqueueCommand(destroyCmd, nullptr);
      command_executor_->WorkerLoop();
    }
  }
}
