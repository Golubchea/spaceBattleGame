#include <BoundingVolumesHierarchy/BoundingVolumesHierarchy.hpp>
#include <cmath>
#include <gtest/gtest.h>
#include <vector>
// Точность сравнения
constexpr float EPS = 1e-5f;

TEST(DynamicBVHTest, EmptyScene_NoCollisions) {
    std::vector<Rect> objects;
    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    EXPECT_TRUE(collisions.empty());
}

TEST(DynamicBVHTest, SingleObject_NoCollisions) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f}
    };
    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    EXPECT_TRUE(collisions.empty());
}

TEST(DynamicBVHTest, TwoStaticObjects_CollisionDetected) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f}, // AABB: (-0.5,-0.5)-(0.5,0.5)
        {{0.5f, 0.5f}, 0.5f, 0.5f, 0.0f}  // AABB: (0,0)-(1,1)
    };
    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    ASSERT_EQ(collisions.size(), 1);
    EXPECT_TRUE((collisions[0] == std::make_pair(0, 1) || collisions[0] == std::make_pair(1, 0)));
}

TEST(DynamicBVHTest, TwoStaticObjects_NoCollision) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f},
        {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f}
    };
    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    EXPECT_TRUE(collisions.empty());
}

TEST(DynamicBVHTest, RotatedObject_AABBUpdated) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, PI / 4.0f} // повернут на 45 градусов
    };

    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    EXPECT_TRUE(collisions.empty());

    // AABB должен быть больше квадрата из-за поворота
    AABB aabb = objects[0].getAABB();
    float size = sqrtf(0.5f * 0.5f * 2); // длина диагонали половины квадрата
    EXPECT_NEAR(aabb.max.x - aabb.min.x, size * 2, EPS);
    EXPECT_NEAR(aabb.max.y - aabb.min.y, size * 2, EPS);
}

TEST(DynamicBVHTest, UpdateMovedObject_DetectedNewCollision) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f},
        {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f}
    };

    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);
    EXPECT_TRUE(collisions.empty());

    // Перемещаем второй объект ближе
    objects[1].center.x = 0.5f;
    objects[1].center.y = 0.5f;
    bvh.update(1);

    collisions.clear();
    bvh.findCollisions(collisions);
    ASSERT_EQ(collisions.size(), 1);
    EXPECT_TRUE((collisions[0] == std::make_pair(0, 1) || collisions[0] == std::make_pair(1, 0)));
}

TEST(DynamicBVHTest, MultipleObjects_CollisionsDetectedCorrectly) {
    std::vector<Rect> objects = {
        {{0.0f, 0.0f}, 0.5f, 0.5f, 0.0f},
        {{0.6f, 0.0f}, 0.5f, 0.5f, 0.0f},
        {{2.0f, 2.0f}, 0.5f, 0.5f, 0.0f},
        {{2.1f, 2.1f}, 0.5f, 0.5f, 0.0f}
    };

    DynamicBVH bvh(objects);

    std::vector<std::pair<int, int>> collisions;
    bvh.findCollisions(collisions);

    EXPECT_EQ(collisions.size(), 2);
    bool found01 = false, found23 = false;
    for (auto& p : collisions) {
        if ((p == std::make_pair(0,1)) || (p == std::make_pair(1,0))) found01 = true;
        if ((p == std::make_pair(2,3)) || (p == std::make_pair(3,2))) found23 = true;
    }
    EXPECT_TRUE(found01);
    EXPECT_TRUE(found23);
}

TEST(DynamicBVHTest, LargeNumberOfObjects_NoCrash) {
  std::vector<Rect> objects;
  for (int i = 0; i < 1000; ++i) {
    float x = static_cast<float>(i % 32) * 0.9f; // Шаг меньше 1 → пересечение
    float y = static_cast<float>(i / 32) * 0.9f;
    objects.push_back(
        {{x, y}, 0.5f, 0.5f, 0.0f}); // Больше шансов на пересечение
  }

  DynamicBVH bvh(objects);

  std::vector<std::pair<int, int>> collisions;
  bvh.findCollisions(collisions);

  EXPECT_GT(collisions.size(), 0);
}
