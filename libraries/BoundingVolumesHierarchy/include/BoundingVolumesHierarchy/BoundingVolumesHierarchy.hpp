#pragma once

#include <memory>
#include <utility>
#include <vector>

const float PI = 3.14159265358979323846f;

struct Vec2 {
    float x, y;
};

struct AABB {
    Vec2 min, max;

    bool intersects(const AABB &other) const;

    static AABB merge(const AABB &a, const AABB &b);

    float surfaceArea() const {
        float dx = max.x - min.x;
        float dy = max.y - min.y;
        return dx * dy;
    }
};

struct Rect {
    Vec2 center;
    float halfWidth, halfHeight; // половины сторон
    float angle; // радианы

    AABB getAABB() const;
};

class DynamicBVH {
public:
  explicit DynamicBVH(std::vector<Rect> &objects) : objects(objects) {
    build();
  }

  void build();
  void update(int objIndex); // обновление одного объекта
  void findCollisions(std::vector<std::pair<int, int>> &collisions);

private:
  struct BVHNode {
    AABB box;
    std::unique_ptr<BVHNode> left, right;
    int index = -1; // только для листьев
    bool isLeaf = false;
  };

  std::vector<Rect> &objects;
  std::unique_ptr<BVHNode> root;
  std::vector<BVHNode *> leaves; // указатели на листья для быстрого доступа

  void buildRecursive(std::vector<int> &indices, BVHNode *node);
  AABB refitNode(BVHNode *node);
};
