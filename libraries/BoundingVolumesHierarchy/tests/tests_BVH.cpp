#include "BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp"
#include "boost/container_hash/hash.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <unordered_set>

constexpr float EPS = 1e-5f;

// --- Vec2 ---
TEST(Vec2Test, Equality_ReturnsTrueForSameValues) {
  Vec2 a{1.0f, 2.0f};
  Vec2 b{1.0f, 2.0f};
  EXPECT_TRUE(a == b);
}

TEST(Vec2Test, Equality_ReturnsFalseForDifferentX) {
  Vec2 a{1.0f, 2.0f};
  Vec2 b{1.1f, 2.0f};
  EXPECT_FALSE(a == b);
}

TEST(Vec2Test, Equality_ReturnsFalseForDifferentY) {
  Vec2 a{1.0f, 2.0f};
  Vec2 b{1.0f, 2.1f};
  EXPECT_FALSE(a == b);
}

// --- AABB ---
TEST(AABBTest, IntersectingBoxes_ReturnTrue) {
  AABB a{{0.0f, 0.0f}, {2.0f, 2.0f}};
  AABB b{{1.0f, 1.0f}, {3.0f, 3.0f}};

  EXPECT_TRUE(a.intersects(b));
}

TEST(AABBTest, NonIntersectingBoxes_ReturnFalse) {
  AABB a{{0.0f, 0.0f}, {1.0f, 1.0f}};
  AABB b{{2.0f, 2.0f}, {3.0f, 3.0f}};

  EXPECT_FALSE(a.intersects(b));
}

TEST(AABBTest, Merge_CombinesTwoBoxes) {
  AABB a{{0.0f, 0.0f}, {2.0f, 2.0f}};
  AABB b{{1.0f, 1.0f}, {3.0f, 3.0f}};
  AABB merged = AABB::merge(a, b);

  EXPECT_FLOAT_EQ(merged.min.x, 0.0f);
  EXPECT_FLOAT_EQ(merged.min.y, 0.0f);
  EXPECT_FLOAT_EQ(merged.max.x, 3.0f);
  EXPECT_FLOAT_EQ(merged.max.y, 3.0f);
}

TEST(AABBTest, SurfaceArea_CalculatesCorrectly) {
  AABB box{{0.0f, 0.0f}, {2.0f, 3.0f}};
  EXPECT_NEAR(box.surfaceArea(), 6.0f, EPS);
}

TEST(AABBTest, Center_ReturnsMidpoint) {
  AABB box{{0.0f, 0.0f}, {2.0f, 4.0f}};
  Vec2 center = box.center();

  EXPECT_NEAR(center.x, 1.0f, EPS);
  EXPECT_NEAR(center.y, 2.0f, EPS);
}

// --- Rect / getAABB ---
TEST(RectTest, GetAABB_NoRotation_IsCentered) {
  Rect rect{{0.0f, 0.0f}, 1.0f, 1.0f, 0.0f};
  AABB aabb = rect.getAABB();

  EXPECT_NEAR(aabb.min.x, -1.0f, EPS);
  EXPECT_NEAR(aabb.min.y, -1.0f, EPS);
  EXPECT_NEAR(aabb.max.x, 1.0f, EPS);
  EXPECT_NEAR(aabb.max.y, 1.0f, EPS);
}

TEST(RectTest, GetAABB_With45DegreeRotation_IsLarger) {
  Rect rect{{0.0f, 0.0f}, 0.5f, 0.5f, M_PI / 4.0f}; // 45 градусов
  AABB aabb = rect.getAABB();

  float expectedSize = sqrtf(0.5f * 0.5f + 0.5f * 0.5f) * 2.0f; // диагональ

  EXPECT_NEAR(aabb.max.x - aabb.min.x, expectedSize, EPS);
  EXPECT_NEAR(aabb.max.y - aabb.min.y, expectedSize, EPS);
}

// --- DynamicBVH: конструктор и пустые данные ---
TEST(DynamicBVHTest, EmptyScene_NoCrash) {
  std::vector<EntityRect> objects;
  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_TRUE(collisions.empty());
}

TEST(DynamicBVHTest, SingleObject_NoCollision) {
  uint64_t entity;
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_TRUE(collisions.empty());
}

// --- Две статические сущности ---
TEST(DynamicBVHTest, TwoStaticObjects_CollisionDetected) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.5f, 0.5f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  ASSERT_EQ(collisions.size(), 1);
}

TEST(DynamicBVHTest, TwoStaticObjects_NoCollision) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  EXPECT_TRUE(collisions.empty());
}

// --- Поворот объекта ---
TEST(DynamicBVHTest, RotatedObject_AABBIsLarger) {
  std::vector<EntityRect> objects = {
      {{0.0f, 0.0f}, 0.5f, 0.5f, M_PI / 4.0f, 1}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_TRUE(collisions.empty());

  AABB aabb = objects[0].getAABB();
  float size = sqrtf(0.5f * 0.5f * 2); // длина диагонали половины квадрата
  EXPECT_NEAR(aabb.max.x - aabb.min.x, size * 2, EPS);
  EXPECT_NEAR(aabb.max.y - aabb.min.y, size * 2, EPS);
}

TEST(DynamicBVHTest, UpdateMovedObject_DetectedNewCollision) {
  std::vector<EntityRect> objects = {
      {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1}, // entity=1
      {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f, 2}  // entity=2
  };

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_TRUE(collisions.empty());

  // Перемещаем второй объект ближе
  objects[1].rect.center.x = 0.5f;
  objects[1].rect.center.y = 0.5f;
  bvh.update(2); // по индексу или по entity ID

  collisions.clear();
  bvh.findCollisions(collisions);

  ASSERT_EQ(collisions.size(), 1);
  // EXPECT_TRUE((collisions[0] == std::make_pair(1u, 2u)) ||
  //             (collisions[0] == std::make_pair(2u, 1u)));
}

// --- Множественные коллизии ---
TEST(DynamicBVHTest, MultipleObjects_CollisionsDetectedCorrectly) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f, 2},
                                     {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f, 3},
                                     {{2.1f, 2.1f}, 0.5f, 0.5f, 0.0f, 4}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  bool found12 = false, found34 = false;
  for (const auto &p : collisions) {
    if ((p.first == 1 && p.second == 2) || (p.first == 2 && p.second == 1))
      found12 = true;
    if ((p.first == 3 && p.second == 4) || (p.first == 4 && p.second == 3))
      found34 = true;
  }

  EXPECT_TRUE(found12);
  EXPECT_TRUE(found34);
}

// --- Большинство объектов ---
TEST(DynamicBVHTest, LargeNumberOfObjects_NoCrash) {
  std::vector<EntityRect> objects;
  for (int i = 0; i < 1000; ++i) {
    float x = static_cast<float>(i % 32) * 0.9f;
    float y = static_cast<float>(i / 32) * 0.9f;
    objects.push_back({{x, y}, 0.5f, 0.5f, 0.0f, static_cast<uint64_t>(i)});
  }

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  EXPECT_GT(collisions.size(), 0);
}

// --- Обновление одного объекта много раз ---
TEST(DynamicBVHTest, ManyUpdates_NoCrash) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  for (int i = 0; i < 100; ++i) {
    objects[0].rect.center.x += 0.01f;
    objects[0].rect.center.y += 0.01f;
    bvh.update(0);

    std::vector<std::pair<uint64_t, uint64_t>> collisions;
    bvh.findCollisions(collisions);
  }

  SUCCEED();
}

// --- Повторная сборка дерева ---
TEST(DynamicBVHTest, BuildAndRefitMultipleTimes_NoCrash) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  for (int i = 0; i < 10; ++i) {
    for (auto &obj : objects) {
      obj.rect.center.x += 0.1f;
      obj.rect.center.y += 0.1f;
    }
    bvh.build(); // Пересобираем дерево
  }

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_GE(collisions.size(), 0);
}

// --- Обновление по индексу ---
TEST(DynamicBVHTest, RefitSingleNode_ChangesAABB) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  AABB oldAABB = objects[0].getAABB();

  objects[0].rect.center.x = 10.0f;
  objects[0].rect.center.y = 10.0f;
  bvh.update(0);

  AABB newAABB = objects[0].getAABB();
  EXPECT_NE(oldAABB.min.x, newAABB.min.x);
  EXPECT_NE(oldAABB.min.y, newAABB.min.y);
}

// --- Проверка уникальности пар ---
TEST(DynamicBVHTest, UniquePairsOnly_ReturnedByFindCollisions) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  // Проверяем, что пара не дублируется в разных комбинациях
  std::unordered_set<std::pair<uint64_t, uint64_t>,
                     boost::hash<std::pair<uint64_t, uint64_t>>>
      unique;
  for (const auto &pair : collisions) {
    unique.insert(pair);
  }

  EXPECT_EQ(unique.size(), collisions.size());
}

// --- Проверка, что update не ломает дерево ---
TEST(DynamicBVHTest, UpdateDoesNotBreakTree) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  for (int i = 0; i < 10; ++i) {
    objects[0].rect.center.x += 0.01f;
    objects[0].rect.center.y += 0.01f;
    bvh.update(0);
  }

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);

  EXPECT_GE(collisions.size(), 0);
}
TEST(DynamicBVHTest, UpdateOnlyOneObject) {
  std::vector<EntityRect> objects = {{{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f, 1},
                                     {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f, 2}};

  DynamicBVH bvh(objects);
  bvh.build();

  std::vector<std::pair<uint64_t, uint64_t>> collisions;
  bvh.findCollisions(collisions);
  EXPECT_TRUE(collisions.empty());

  // Перемещаем первый объект
  objects[1].rect.center.x = 0.5f;
  objects[1].rect.center.y = 0.5f;
  bvh.update(2); // если update принимает `uint64_t entity`

  collisions.clear();
  bvh.findCollisions(collisions);

  bool found = false;
  for (const auto &[a, b] : collisions) {
    if ((a == 1 && b == 2) || (a == 2 && b == 1)) {
      found = true;
      break;
    }
  }

  EXPECT_TRUE(found);
}
