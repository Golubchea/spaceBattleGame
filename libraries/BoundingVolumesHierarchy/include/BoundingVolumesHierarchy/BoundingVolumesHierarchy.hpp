// BoundingVolumesHierarchy.hpp
#pragma once

#include <memory>
#include <utility>
#include <vector>

struct Vec2 {
  float x, y;

  bool operator==(const Vec2 &other) const {
    return x == other.x && y == other.y;
  }
};

struct AABB {
  Vec2 min, max;

  bool intersects(const AABB &other) const;
  static AABB merge(const AABB &a, const AABB &b);
  float surfaceArea() const;

  Vec2 center() const {
    return {(min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f};
  }
};

struct Rect {
  Vec2 center;
  float halfWidth, halfHeight;
  float angle; // радианы

  AABB getAABB() const;
};

struct EntityRect {
  Rect rect;
  uint64_t entity;
  EntityRect(Vec2 center, float halfWidth, float halfHeight, float angle,
             uint64_t e)
      : rect{center, halfWidth, halfHeight, angle}, entity(e) {}

  EntityRect(Rect rect_, uint64_t e) : rect(rect_), entity(e) {}
  AABB getAABB() const { return rect.getAABB(); }
};
struct BVHNode {
  AABB box;
  std::unique_ptr<BVHNode> left, right;
  int index = -1; // индекс в массиве объектов
  bool isLeaf = false;
};
class DynamicBVH {
public:
  explicit DynamicBVH(std::vector<EntityRect> &objects);
  int findObjectIndex(uint64_t entityID) const {
    for (size_t i = 0; i < objects_.size(); ++i) {
      if (objects_[i].entity == entityID) {
        return static_cast<int>(i);
      }
    }
    return -1; // Не найден
  }

  void update(uint64_t entityID) {
    int index = findObjectIndex(entityID);
    if (index >= 0) {
      refitNode(leaves_[index]);
    }
  }
  void build();

  void findCollisions(std::vector<std::pair<uint64_t, uint64_t>> &collisions);

  std::vector<EntityRect> &objects() const;
  void setObjects(const std::vector<EntityRect> &newObjects);

private:
  std::vector<EntityRect> &objects_;
  std::unique_ptr<BVHNode> root_;
  std::vector<BVHNode *> leaves_;

  void buildRecursive(std::vector<int> &indices, BVHNode *node);
  AABB refitNode(BVHNode *node);
};
