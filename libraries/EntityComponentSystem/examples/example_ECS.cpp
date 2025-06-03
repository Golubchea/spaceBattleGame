#include "ECSEngineLib/Components/FuelComponent.hpp"
#include "ECSEngineLib/Components/VelocityComponent.hpp"
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/World.hpp>
#include <boost/serialization/extended_type_info_typeid.hpp>
#include <iostream>

int main(int, char **) {

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
  std::cout << "here\n" << std::flush;

  std::cout << "Registered types:\n";
  world.Save(filename);

  std::cout << "here\n" << std::flush;
  // Создание нового мира и загрузка данных
  World newWorld;
  newWorld.Load(filename);

  // Проверка восстановленных данных
  auto pos1 = newWorld.GetComponent<PositionComponent>(e1);
  auto vel1 = newWorld.GetComponent<VelocityComponent>(e1);
  auto fuel1 = newWorld.GetComponent<FuelComponent>(e1);

  auto pos2 = newWorld.GetComponent<PositionComponent>(e2);
  auto vel2 = newWorld.GetComponent<VelocityComponent>(e2);

  return 0;
}
