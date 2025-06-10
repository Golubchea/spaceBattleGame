#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <cmath>
#include <iostream>

const float PI = 3.14159265358979323846f;

int main() {
  std::vector<EntityRect> objects = {
      {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},    // объект 1
      {{0.5f, 0.5f}, 0.5f, 0.5f, M_PI / 3, 2} // объект 2, повернутый
  };
  DynamicBVH dbvh(objects);
  dbvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  dbvh.findCollisions(collisions);

  std::cout << "Initial collisions:\n";
  for (const auto &[a, b] : collisions) {
    std::cout << "Object with ID " << a << " and " << b << " collide\n";
  }

  // === Обновляем позицию одного объекта ===
  objects[0].rect.center.x += 12.0f;
  objects[0].rect.center.y += 12.0f;

  // Обновляем только этот объект в BVH
  dbvh.update(0); // передаем индекс вектора

  // Ищем коллизии снова
  collisions.clear();
  dbvh.findCollisions(collisions);

  std::cout << "\nAfter moving object ID=1:\n";
  if (collisions.empty()) {
    std::cout << "No collisions found.\n";
  } else {
    for (const auto &[a, b] : collisions) {
      std::cout << "Object with ID " << a << " and " << b << " collide\n";
    }
  }

  return 0;
}
