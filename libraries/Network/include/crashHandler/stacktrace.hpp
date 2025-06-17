#pragma once
#include <functional>
#include <memory>

class IStackTrace {
public:
  virtual void init(std::function<void()> func) = 0;
};

class StackTraceFactory {
public:
  static std::shared_ptr<IStackTrace> createStackTrace();
};
