#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <algorithm>
#include <cmath>
#include <stack>
#include <unordered_set>

// === Реализация Rect и AABB ===
AABB Rect::getAABB() const {
  AABB aabb;
  float cosA = std::cos(angle);
  float sinA = std::sin(angle);

  float hw = halfWidth;
  float hh = halfHeight;

  float points[4][2] = {{hw * cosA - hh * sinA, hw * sinA + hh * cosA},
                        {-hw * cosA - hh * sinA, -hw * sinA + hh * cosA},
                        {-hw * cosA + hh * sinA, -hw * sinA - hh * cosA},
                        {hw * cosA + hh * sinA, hw * sinA - hh * cosA}};

  aabb.min.x = aabb.max.x = center.x;
  aabb.min.y = aabb.max.y = center.y;

  for (int i = 0; i < 4; ++i) {
    float x = center.x + points[i][0];
    float y = center.y + points[i][1];

    aabb.min.x = std::min(aabb.min.x, x);
    aabb.min.y = std::min(aabb.min.y, y);
    aabb.max.x = std::max(aabb.max.x, x);
    aabb.max.y = std::max(aabb.max.y, y);
  }

  return aabb;
}

bool AABB::intersects(const AABB &other) const {
  if (max.x <= other.min.x || other.max.x <= min.x)
    return false;
  if (max.y <= other.min.y || other.max.y <= min.y)
    return false;
  return true;
}

AABB AABB::merge(const AABB &a, const AABB &b) {
  AABB result;
  result.min.x = std::min(a.min.x, b.min.x);
  result.min.y = std::min(a.min.y, b.min.y);
  result.max.x = std::max(a.max.x, b.max.x);
  result.max.y = std::max(a.max.y, b.max.y);
  return result;
}

float AABB::surfaceArea() const {
  float dx = max.x - min.x;
  float dy = max.y - min.y;
  return dx * dy;
}
// === Реализация DynamicBVH ===
DynamicBVH::DynamicBVH(std::vector<EntityRect> &objects)
    : objects_(objects), root_(std::make_unique<BVHNode>()) {}

void DynamicBVH::build() {
  leaves_.clear();
  std::vector<int> indices(objects_.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    indices[i] = static_cast<int>(i);
  }

  buildRecursive(indices, root_.get());
}

void DynamicBVH::buildRecursive(std::vector<int> &indices, BVHNode *node) {
  if (indices.empty())
    return;

  if (indices.size() == 1) {
    node->isLeaf = true;
    node->index = indices[0];
    node->box = objects_[indices[0]].getAABB();
    leaves_.push_back(node);
    return;
  }

  // Объединяем AABB всех объектов
  node->box = {};
  for (int idx : indices) {
    node->box = AABB::merge(node->box, objects_[idx].getAABB());
  }

  auto mid = indices.begin() + indices.size() / 2;
  std::nth_element(indices.begin(), mid, indices.end(), [&](int a, int b) {
    return objects_[a].rect.center.x < objects_[b].rect.center.x;
  });

  std::vector<int> leftIndices(indices.begin(), mid);
  std::vector<int> rightIndices(mid, indices.end());

  node->left = std::make_unique<BVHNode>();
  node->right = std::make_unique<BVHNode>();

  buildRecursive(leftIndices, node->left.get());
  buildRecursive(rightIndices, node->right.get());

  node->box = AABB::merge(node->left->box, node->right->box);
}

AABB DynamicBVH::refitNode(BVHNode *node) {
  if (!node)
    return {};

  if (node->isLeaf) {
    node->box = objects_[node->index].getAABB();
    return node->box;
  } else {
    AABB leftBox = refitNode(node->left.get());
    AABB rightBox = refitNode(node->right.get());
    node->box = AABB::merge(leftBox, rightBox);
    return node->box;
  }
}

void DynamicBVH::findCollisions(
    std::vector<std::pair<uint64_t, uint64_t>> &collisions) {
  collisions.clear();
  if (!root_)
    return;

  std::stack<std::pair<BVHNode *, BVHNode *>> stack;
  std::unordered_set<uint64_t> seen;

  stack.push({root_.get(), root_.get()});

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
      if (i > j)
        std::swap(i, j);

      uint64_t key = ((uint64_t)i << 32) | j;
      if (!seen.count(key)) {
        collisions.emplace_back(objects_[i].entity, objects_[j].entity);
        seen.insert(key);
      }
    } else if (a->isLeaf) {
      if (a->left)
        stack.push({a, a->left.get()});
      if (a->right)
        stack.push({a, a->right.get()});
    } else if (b->isLeaf) {
      if (b->left)
        stack.push({b->left.get(), b});
      if (b->right)
        stack.push({b->right.get(), b});
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

std::vector<EntityRect> &DynamicBVH::objects() const { return objects_; }

void DynamicBVH::setObjects(const std::vector<EntityRect> &newObjects) {
  objects_ = newObjects;
}
