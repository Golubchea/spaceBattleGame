#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <iostream>

int main() {
  std::vector<Rect> objects = {
      {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f}, // не повернут
      // {{1.0f, 0.0f}, 0.5f, 0.5f, PI / 4},           // 45 градусов
      // {{2.0f, 2.0f}, 0.5f, 0.5f, PI / 2},           // 90 градусов
      {{0.5f, 0.5f}, 0.5f, 0.5f, PI / 3} // пересекается с первым
  };

  DynamicBVH dbvh(objects);

  std::vector<std::pair<int, int>> collisions;
  dbvh.findCollisions(collisions);

  std::cout << "Initial collisions:\n";
  for (auto [i, j] : collisions) {
    std::cout << "Object " << i << " and " << j << "\n";
  }

    // Изменяем позицию одного объекта
    objects[0].center.x += 12.0f;
    objects[0].center.y += 12.0f;

    // Обновляем только этот объект в BVH
    dbvh.update(0);

    // Ищем снова
    collisions.clear();
    dbvh.findCollisions(collisions);

    std::cout << "\nAfter moving object 3:\n";
    for (auto [i, j] : collisions) {
        std::cout << "Object " << i << " and " << j << "\n";
    }

    return 0;
}
