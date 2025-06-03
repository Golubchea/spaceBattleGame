#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stack>
#include <unordered_set>

void DynamicBVH::build() {
  leaves.clear();
  root = std::make_unique<BVHNode>();

  std::vector<int> indices(objects.size());
  for (int i = 0; i < (int)indices.size(); ++i)
    indices[i] = i;

  buildRecursive(indices, root.get());
}

bool AABB::intersects(const AABB &other) const {
  return !(max.x < other.min.x || other.max.x < min.x || max.y < other.min.y ||
           other.max.y < min.y);
}

AABB AABB::merge(const AABB &a, const AABB &b) {
  AABB result;
  result.min.x = std::min(a.min.x, b.min.x);
  result.min.y = std::min(a.min.y, b.min.y);
  result.max.x = std::max(a.max.x, b.max.x);
  result.max.y = std::max(a.max.y, b.max.y);
  return result;
}

// float AABB::surfaceArea() const {
//   float dx = max.x - min.x;
//   float dy = max.y - min.y;
//   return dx * dy;
// }

void DynamicBVH::buildRecursive(std::vector<int> &indices, BVHNode *node) {
  if (indices.empty()) {
    // Если индексы пустые, ничего не делаем
    return;
  }

  if (indices.size() == 1) {
    node->isLeaf = true;
    node->index = indices[0];
    node->box = objects[indices[0]].getAABB();
    leaves.push_back(node);
    return;
  }

  // Вычисляем общий AABB
  AABB totalBox;
  totalBox.min.x = totalBox.min.y = std::numeric_limits<float>::max();
  totalBox.max.x = totalBox.max.y = -std::numeric_limits<float>::max();

  for (int idx : indices) {
    AABB objBox = objects[idx].getAABB();
    totalBox.min.x = std::min(totalBox.min.x, objBox.min.x);
    totalBox.min.y = std::min(totalBox.min.y, objBox.min.y);
    totalBox.max.x = std::max(totalBox.max.x, objBox.max.x);
    totalBox.max.y = std::max(totalBox.max.y, objBox.max.y);
  }

  int axis =
      (totalBox.max.x - totalBox.min.x) > (totalBox.max.y - totalBox.min.y) ? 0
                                                                            : 1;

  // Разделение по среднему центру
  std::sort(indices.begin(), indices.end(), [&](int i1, int i2) {
    if (axis == 0)
      return objects[i1].center.x < objects[i2].center.x;
    else
      return objects[i1].center.y < objects[i2].center.y;
  });

  size_t mid = indices.size() / 2;
  std::vector<int> leftIndices(indices.begin(), indices.begin() + mid);
  std::vector<int> rightIndices(indices.begin() + mid, indices.end());

  node->left = std::make_unique<BVHNode>();
  node->right = std::make_unique<BVHNode>();

  buildRecursive(leftIndices, node->left.get());
  buildRecursive(rightIndices, node->right.get());

  node->box = AABB::merge(node->left->box, node->right->box);
}

AABB DynamicBVH::refitNode(DynamicBVH::BVHNode *node) {
  if (node->isLeaf) {
    node->box = objects[node->index].getAABB();
    return node->box;
  } else {
    AABB leftBox = refitNode(node->left.get());
    AABB rightBox = refitNode(node->right.get());
    node->box = AABB::merge(leftBox, rightBox);
    return node->box;
  }
}

void DynamicBVH::update(int objIndex) {
  for (auto leaf : leaves) {
    if (leaf->index == objIndex) {
      refitNode(leaf);
      break;
    }
  }
}
void DynamicBVH::findCollisions(std::vector<std::pair<int, int>> &collisions) {
  if (!root)
    return;

  std::stack<std::pair<BVHNode *, BVHNode *>> stack;
  std::unordered_set<uint64_t> seen; // Для отслеживания уникальных пар

  stack.push({root.get(), root.get()});

  while (!stack.empty()) {
    auto [a, b] = stack.top();
    stack.pop();

    if (!a || !b)
      continue;

    if (!a->box.intersects(b->box))
      continue;

    if (a->isLeaf && b->isLeaf && a != b) {
      int i = a->index;
      int j = b->index;

      if (i == j)
        continue;

      // Гарантируем порядок: i < j
      if (i > j)
        std::swap(i, j);

      uint64_t key = ((uint64_t)i << 32) | j;
      if (!seen.contains(key)) {
        collisions.emplace_back(i, j);
        seen.insert(key);
      }
      continue;
    }

    if (a->isLeaf) {
      if (b->left)
        stack.push({a, b->left.get()});
      if (b->right)
        stack.push({a, b->right.get()});
    } else if (b->isLeaf) {
      if (a->left)
        stack.push({a->left.get(), b});
      if (a->right)
        stack.push({a->right.get(), b});
    } else {
      if (a->left && b->left)
        stack.push({a->left.get(), b->left.get()});
      if (a->left && b->right)
        stack.push({a->left.get(), b->right.get()});
      if (a->right && b->left)
        stack.push({a->right.get(), b->left.get()});
      if (a->right && b->right)
        stack.push({a->right.get(), b->right.get()});
    }
  }
}

AABB Rect::getAABB() const {
  std::vector<Vec2> corners = {{halfWidth, halfHeight},
                               {-halfWidth, halfHeight},
                               {-halfWidth, -halfHeight},
                               {halfWidth, -halfHeight}};

  float cosA = std::cos(angle);
  float sinA = std::sin(angle);

  AABB aabb;
  aabb.min.x = aabb.min.y = std::numeric_limits<float>::max();
  aabb.max.x = aabb.max.y = -std::numeric_limits<float>::max();

  for (const auto &v : corners) {
    float x = v.x * cosA - v.y * sinA + center.x;
    float y = v.x * sinA + v.y * cosA + center.y;

    aabb.min.x = std::min(aabb.min.x, x);
    aabb.min.y = std::min(aabb.min.y, y);
    aabb.max.x = std::max(aabb.max.x, x);
    aabb.max.y = std::max(aabb.max.y, y);
  }

  return aabb;
}
