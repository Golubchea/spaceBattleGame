#pragma once
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

class Signal {
public:
  using Callback = std::function<void(void)>;

  void connect(std::string type, Callback cb);

  void emit(std::string type);

  void disconnectAll();

private:
  std::unordered_map<std::string, Callback> callbacks_;
  std::mutex mutex_;
};
