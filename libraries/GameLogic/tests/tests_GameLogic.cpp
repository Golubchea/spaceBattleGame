
#include "gtest/gtest.h"
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/VelocityComponent.hpp>
#include <ECSEngineLib/World.hpp>
#include <ServerLogic/Player.hpp>
#include <gtest/gtest.h>

TEST(PlayerTest, CreatePlayerWithEntity) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  EXPECT_EQ(player.entity(), entity);
}

TEST(PlayerTest, SetAndGetPosition_Property) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  Vector<double, 2> expectedPos{1.0, 2.0};
  std::array<double, 2> arr{expectedPos.x(), expectedPos.y()};
  std::vector<uint8_t> data(arr.data(), arr.data() + sizeof(arr));

  player.setPosition(expectedPos);

  auto result = player.GetProperty("position");
  ASSERT_EQ(result.size(), sizeof(arr));
  auto pos = reinterpret_cast<const std::array<double, 2> *>(result.data());

  EXPECT_DOUBLE_EQ((*pos)[0], expectedPos.x());
  EXPECT_DOUBLE_EQ((*pos)[1], expectedPos.y());

  // Проверяем через интерфейс IMovingObject
  EXPECT_DOUBLE_EQ(player.getPosition().x(), expectedPos.x());
  EXPECT_DOUBLE_EQ(player.getPosition().y(), expectedPos.y());
}

TEST(PlayerTest, HasPosition_Property) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  EXPECT_FALSE(player.HasProperty("position"));

  Vector<double, 2> expectedPos{3.0, 4.0};
  std::array<double, 2> arr{expectedPos.x(), expectedPos.y()};
  std::vector<uint8_t> data(arr.data(), arr.data() + sizeof(arr));
  player.SetProperty("position", data);

  EXPECT_TRUE(player.HasProperty("position"));
}

TEST(PlayerTest, GetPosition_WhenNoComponent_ReturnsZero) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  Vector<double, 2> pos = player.getPosition();
  EXPECT_DOUBLE_EQ(pos.x(), 0.0);
  EXPECT_DOUBLE_EQ(pos.y(), 0.0);
}

TEST(PlayerTest, SetPosition_UpdatesComponent) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  Vector<double, 2> newPos{5.0, 6.0};
  player.setPosition(newPos);

  auto comp = world->GetComponent<PositionComponent>(entity);
  ASSERT_NE(comp, nullptr);
  EXPECT_DOUBLE_EQ(comp->getPosition()[0], newPos.x());
  EXPECT_DOUBLE_EQ(comp->getPosition()[1], newPos.y());
}

TEST(PlayerTest, Clear_RemovesEntityFromWorld) {
  auto world = std::make_shared<World>();
  Entity entity = world->CreateEntity();
  Player player(entity, world);

  player.Clear();

  auto comp = world->GetComponent<PositionComponent>(entity);
  EXPECT_EQ(comp, nullptr);
  EXPECT_FALSE(world->HasComponent<PositionComponent>(entity));
}

TEST(PlayerTest, ChangeEntity_UpdatesInternalReference) {
  auto world = std::make_shared<World>();

  Entity entity1 = world->CreateEntity();
  Entity entity2 = world->CreateEntity();
  Player player(entity1, world);

  player.setEntity(entity2);

  EXPECT_EQ(player.entity(), entity2);
}
