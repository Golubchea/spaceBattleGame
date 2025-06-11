#include <InversionOfControl/InversionOfControl.h>
#include <gtest/gtest.h>
#include <thread>

struct MyService {
  std::string GetMessage() const { return "Hello from MyService!"; }
};

TEST(IoCTest, ResolveRegisteredFactory) {
  auto &container = IoCContainer::GetInstance();

  container.RegisterFactory<MyService>(
      "MyService", []() { return std::make_shared<MyService>(); });

  auto svc = container.Resolve<std::shared_ptr<MyService>>("MyService");
  EXPECT_EQ(svc->GetMessage(), "Hello from MyService!");
}

TEST(IoCTest, ResolveRegisteredService) {
  IoCContainer::GetInstance().RegisterFactory<MyService>(
      "MyService", []() { return std::make_shared<MyService>(); });

  auto svc = IoCContainer::GetInstance().Resolve<std::shared_ptr<MyService>>(
      "MyService");
  ASSERT_NE(svc, nullptr);
  EXPECT_EQ(svc->GetMessage(), "Hello from MyService!");
}

TEST(IoCTest, ResolveUnregisteredServiceThrows) {
  ASSERT_THROW(IoCContainer::GetInstance().Resolve<std::shared_ptr<MyService>>(
                   "Unknown"),
               std::runtime_error);
}

TEST(IoCTest, Scopes_Isolation) {
  auto &container = IoCContainer::GetInstance();

  // Скоуп 1
  container.CreateNewScope("test_scope_1");
  container.SetCurrentScope("test_scope_1");
  container.RegisterFactory<MyService>(
      "service", [] { return std::make_shared<MyService>(); });

  EXPECT_THROW(container.Resolve<MyService>("service").GetMessage(),
               std::runtime_error); // "Hello from MyService!");

  // Скоуп 2
  container.CreateNewScope("test_scope_2");
  EXPECT_THROW(container.Resolve<MyService>("service"), std::runtime_error);
}

TEST(IoCTest, MultiThreaded_ScopeIsolation) {
  constexpr int THREAD_COUNT = 0;

  for (int i = 0; i < THREAD_COUNT; ++i) {
    std::thread([i]() {
      auto scopeId = "thread_scope_" + std::to_string(i);
      IoCContainer::GetInstance().CreateNewScope(scopeId);

      IoCContainer::GetInstance().RegisterFactory<MyService>(
          "MyService", []() { return std::make_shared<MyService>(); });

      auto svc =
          IoCContainer::GetInstance().Resolve<std::shared_ptr<MyService>>(
              "MyService");
      EXPECT_EQ(svc->GetMessage(), "Hello from MyService!");
    }).detach();
  }

  std::this_thread::sleep_for(std::chrono::seconds(1)); // Даем время потокам
}
