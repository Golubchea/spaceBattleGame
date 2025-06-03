#pragma once

#include <any>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class IoCContainer {
private:
  using Resolver = std::function<std::any(const std::vector<std::any> &args)>;

  struct Scope {
    std::unordered_map<std::string, Resolver> services;
  };

  std::unordered_map<std::thread::id, std::unordered_map<std::string, Scope>>
      scopes_;
  std::string currentScopeId_ = "default";
  mutable std::mutex mutex_;

public:
  static IoCContainer &GetInstance() {
    static IoCContainer instance;
    return instance;
  }

  template <typename T>
  void Register(const std::string &key,
                std::function<T(const std::vector<std::any> &args)> resolver) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto threadId = std::this_thread::get_id();
    auto &scope = scopes_[threadId][currentScopeId_];

    scope.services[key] =
        [resolver](const std::vector<std::any> &args) -> std::any {
      return resolver(args);
    };
  }

  template <typename T>
  void RegisterFactory(const std::string &key,
                       std::function<std::shared_ptr<T>()> factory) {
    Register<std::shared_ptr<T>>(
        key,
        [factory](const std::vector<std::any> &args) { return factory(); });
  }

  template <typename T>
  T Resolve(const std::string &key, const std::vector<std::any> &args = {}) {
    // Сначала копируем указатель под защитой мьютекса
    Resolver resolver;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      auto threadId = std::this_thread::get_id();

      auto threadIt = scopes_.find(threadId);
      if (threadIt == scopes_.end()) {
        throw std::runtime_error("No scopes exist for this thread");
      }

      auto scopeIt = threadIt->second.find(currentScopeId_);
      if (scopeIt == threadIt->second.end()) {
        throw std::runtime_error("Current scope does not exist in this thread");
      }

      auto serviceIt = scopeIt->second.services.find(key);
      if (serviceIt == scopeIt->second.services.end()) {
        throw std::runtime_error("Service not found: " + key);
      }

      resolver = serviceIt->second;
    }

    // Теперь вызываем вне lock'а
    try {
      return std::any_cast<T>(resolver(args));
    } catch (...) {
      throw std::runtime_error(
          "Failed to cast resolved service to requested type.");
    }
  }

  void CreateNewScope(const std::string &scopeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto threadId = std::this_thread::get_id();
    scopes_[threadId][scopeId] = Scope{};
    currentScopeId_ = scopeId;
  }

  void SetCurrentScope(const std::string &scopeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto threadId = std::this_thread::get_id();

    auto threadIt = scopes_.find(threadId);
    if (threadIt == scopes_.end() ||
        threadIt->second.find(scopeId) == threadIt->second.end()) {
      throw std::runtime_error("Scope does not exist: " + scopeId);
    }

    currentScopeId_ = scopeId;
  }

private:
  IoCContainer() = default;
};
