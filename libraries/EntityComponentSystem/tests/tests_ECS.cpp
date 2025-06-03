#include "ECSEngineLib/Components/FuelComponent.hpp"
#include "ECSEngineLib/Components/VelocityComponent.hpp"
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/World.hpp>
#include <gtest/gtest.h>

TEST(WorldTest, SetAndGetFuel) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<FuelComponent>(e1, 100.0);

  auto fuel = world.GetComponent<FuelComponent>(e1);
  ASSERT_TRUE(fuel != nullptr);

  EXPECT_EQ(fuel->getFuel(), 100.0);
  fuel->setFuel(50.0);
  EXPECT_EQ(fuel->getFuel(), 50.0);
}

TEST(WorldTest, SetAndGetPosition) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{0.0, 0.0});

  auto pos = world.GetComponent<PositionComponent>(e1);
  ASSERT_TRUE(pos != nullptr);

  EXPECT_EQ(pos->getPosition()[0], 0.0);
  EXPECT_EQ(pos->getPosition()[1], 0.0);

  pos->setPosition({3.0, 4.0});
  EXPECT_EQ(pos->getPosition()[0], 3.0);
  EXPECT_EQ(pos->getPosition()[1], 4.0);
}

TEST(WorldTest, SetAndGetVelocity) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<VelocityComponent>(e1, std::array<double, 2>{0.0, 0.0});

  auto vel = world.GetComponent<VelocityComponent>(e1);
  ASSERT_TRUE(vel != nullptr);

  EXPECT_EQ(vel->getVelocity()[0], 0.0);
  EXPECT_EQ(vel->getVelocity()[1], 0.0);

  vel->setVelocity({1.5, 2.5});
  EXPECT_EQ(vel->getVelocity()[0], 1.5);
  EXPECT_EQ(vel->getVelocity()[1], 2.5);
}

TEST(WorldTest, UpdateIsCalled) {
  World world;
  // Просто проверяем, что метод вызывается без ошибок
  EXPECT_NO_THROW(world.Update(0.016f)); // 60 FPS delta time
}
struct DummyComponent : public Component {
  int value;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar & value;
  }
};

BOOST_CLASS_EXPORT_GUID(DummyComponent, "DummyComponent")

TEST(WorldTest, HasComponent_UnknownComponent) {
  World world;
  Entity e1 = world.CreateEntity();

  // Проверяем, что компонента DummyComponent вообще не существует
  EXPECT_FALSE(world.HasComponent<DummyComponent>(e1));
}

TEST(WorldTest, GetComponentReturnsNullptrForUnknownType) {
  World world;
  Entity e1 = world.CreateEntity();

  auto dummy = world.GetComponent<DummyComponent>(e1);
  EXPECT_EQ(dummy, nullptr);
}

TEST(WorldTest, GetComponentFromNonExistentEntity2) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  Entity invalid_entity = 999; // Такой сущности нет
  auto pos = world.GetComponent<PositionComponent>(invalid_entity);
  EXPECT_EQ(pos, nullptr); // Не должно быть найдено
}
TEST(WorldTest, GetComponentThatDoesNotExist2) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  // У e1 нет VelocityComponent
  auto vel = world.GetComponent<VelocityComponent>(e1);
  EXPECT_EQ(vel, nullptr); // Не должно быть найдено
}

TEST(WorldTest, GetComponentUnknownType2) {
  World world;
  Entity e1 = world.CreateEntity();

  auto dummy = world.GetComponent<DummyComponent>(e1);
  EXPECT_EQ(dummy, nullptr); // Не должно быть найдено
}

TEST(WorldTest, HasComponentReturnsFalseIfTypeNotRegistered) {
  World world;
  Entity e1 = world.CreateEntity();

  // Запрашиваем компонент, которого никогда не добавляли
  EXPECT_FALSE(world.HasComponent<VelocityComponent>(e1));
}
TEST(WorldTest, GetComponent_UnknownComponent) {
  World world;
  Entity e1 = world.CreateEntity();

  // Проверяем, что компонента DummyComponent вообще не существует
  EXPECT_EQ(world.GetComponent<DummyComponent>(e1), nullptr);
}
TEST(WorldTest, GetComponentAfterModify) {
  World world;
  Entity e1 = world.CreateEntity();

  // Добавляем и изменяем позицию
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});
  auto pos = world.GetComponent<PositionComponent>(e1);
  ASSERT_TRUE(pos != nullptr);
  pos->setPosition({3.0, 4.0});
  EXPECT_EQ(pos->getPosition()[0], 3.0);
  EXPECT_EQ(pos->getPosition()[1], 4.0);

  // Добавляем и изменяем скорость
  world.AddComponent<VelocityComponent>(e1, std::array<double, 2>{0.0, 0.0});
  auto vel = world.GetComponent<VelocityComponent>(e1);
  ASSERT_TRUE(vel != nullptr);
  vel->setVelocity({5.0, 6.0});
  EXPECT_EQ(vel->getVelocity()[0], 5.0);
  EXPECT_EQ(vel->getVelocity()[1], 6.0);
}

TEST(WorldTest, CreateEntity) {
  World world;
  Entity e1 = world.CreateEntity();
  Entity e2 = world.CreateEntity();
  EXPECT_EQ(e1, 1); // Первый ID должен быть 1
  EXPECT_EQ(e2, 2); // Второй ID должен быть 2
}

TEST(WorldTest, AddComponent) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  auto pos = world.GetComponent<PositionComponent>(e1);
  ASSERT_TRUE(pos != nullptr);
  EXPECT_EQ(pos->getPosition()[0], 1.0);
  EXPECT_EQ(pos->getPosition()[1], 2.0);
}

TEST(WorldTest, DestroyEntity) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});
  world.DestroyEntity(e1);

  // Проверяем, что компонент удален
  auto pos = world.GetComponent<PositionComponent>(e1);
  EXPECT_TRUE(pos == nullptr);
}

TEST(WorldTest, HasComponent) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  EXPECT_TRUE(world.HasComponent<PositionComponent>(e1));
  EXPECT_FALSE(world.HasComponent<VelocityComponent>(e1));
}

TEST(WorldTest, GetComponent) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  auto pos = world.GetComponent<PositionComponent>(e1);
  ASSERT_TRUE(pos != nullptr);
  EXPECT_EQ(pos->getPosition()[0], 1.0);
  EXPECT_EQ(pos->getPosition()[1], 2.0);

  auto vel = world.GetComponent<VelocityComponent>(e1);
  EXPECT_TRUE(vel == nullptr);
}

TEST(WorldTest, MultipleComponents) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});
  world.AddComponent<VelocityComponent>(e1, std::array<double, 2>{3.0, 4.0});

  EXPECT_TRUE(world.HasComponent<PositionComponent>(e1));
  EXPECT_TRUE(world.HasComponent<VelocityComponent>(e1));

  auto pos = world.GetComponent<PositionComponent>(e1);
  auto vel = world.GetComponent<VelocityComponent>(e1);

  ASSERT_TRUE(pos != nullptr);
  ASSERT_TRUE(vel != nullptr);

  EXPECT_EQ(pos->getPosition()[0], 1.0);
  EXPECT_EQ(pos->getPosition()[1], 2.0);
  EXPECT_EQ(vel->getVelocity()[0], 3.0);
  EXPECT_EQ(vel->getVelocity()[1], 4.0);
}

TEST(ECS, SerializeAndDeserializeWorld) {
  // Создание мира
  World world;

  // Создание сущностей
  Entity e1 = world.CreateEntity();
  Entity e2 = world.CreateEntity();

  // Добавление компонентов
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});
  world.AddComponent<VelocityComponent>(e1, std::array<double, 2>{3.0, 4.0});
  world.AddComponent<FuelComponent>(e1, 100.0);

  world.AddComponent<PositionComponent>(e2, std::array<double, 2>{5.0, 6.0});
  world.AddComponent<VelocityComponent>(e2, std::array<double, 2>{7.0, 8.0});

  // Сохранение мира в файл
  const std::string filename = "world_test.dat";

  world.Save(filename);

  // Создание нового мира и загрузка данных
  World newWorld;
  newWorld.Load(filename);

  // Проверка восстановленных данных
  auto pos1 = newWorld.GetComponent<PositionComponent>(e1);
  auto vel1 = newWorld.GetComponent<VelocityComponent>(e1);
  auto fuel1 = newWorld.GetComponent<FuelComponent>(e1);

  auto pos2 = newWorld.GetComponent<PositionComponent>(e2);
  auto vel2 = newWorld.GetComponent<VelocityComponent>(e2);

  // Проверка первой сущности
  ASSERT_TRUE(pos1 != nullptr);
  EXPECT_EQ(pos1->getPosition()[0], 1.0);
  EXPECT_EQ(pos1->getPosition()[1], 2.0);

  ASSERT_TRUE(vel1 != nullptr);
  EXPECT_EQ(vel1->getVelocity()[0], 3.0);
  EXPECT_EQ(vel1->getVelocity()[1], 4.0);

  ASSERT_TRUE(fuel1 != nullptr);
  EXPECT_EQ(fuel1->getFuel(), 100.0);

  // Проверка второй сущности
  ASSERT_TRUE(pos2 != nullptr);
  EXPECT_EQ(pos2->getPosition()[0], 5.0);
  EXPECT_EQ(pos2->getPosition()[1], 6.0);

  ASSERT_TRUE(vel2 != nullptr);
  EXPECT_EQ(vel2->getVelocity()[0], 7.0);
  EXPECT_EQ(vel2->getVelocity()[1], 8.0);
}

TEST(WorldTest, GetComponentFromNonExistentEntity) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  Entity invalid_entity = 999; // Такой сущности нет
  auto pos = world.GetComponent<PositionComponent>(invalid_entity);
  EXPECT_TRUE(pos == nullptr); // Не должно быть найдено
}

TEST(WorldTest, GetComponentThatDoesNotExist) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  // Запрашиваем компонент, которого нет у этой сущности
  auto vel = world.GetComponent<VelocityComponent>(e1);
  EXPECT_TRUE(vel == nullptr); // Не должно быть найдено
}

TEST(WorldTest, GetComponentAfterDestroyEntity) {
  World world;
  Entity e1 = world.CreateEntity();
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  world.DestroyEntity(e1);

  auto pos = world.GetComponent<PositionComponent>(e1);
  EXPECT_TRUE(pos == nullptr); // Компонент должен быть удален
}
 

TEST(WorldTest, HasComponentReturnsFalseForNonExistentComponent) {
  World world;
  Entity e1 = world.CreateEntity();

  // Убедись, что у сущности нет VelocityComponent
  EXPECT_FALSE(world.HasComponent<VelocityComponent>(e1));

  // Добавляем PositionComponent
  world.AddComponent<PositionComponent>(e1, std::array<double, 2>{1.0, 2.0});

  // Убедись, что по-прежнему нет VelocityComponent
  EXPECT_FALSE(world.HasComponent<VelocityComponent>(e1));
}

TEST(WorldTest, GetComponentUnknownType) {
  World world;
  Entity e1 = world.CreateEntity();

  // Ни один из этих типов не добавлен
  auto dummy = world.GetComponent<DummyComponent>(e1);
  EXPECT_TRUE(dummy == nullptr);
}
