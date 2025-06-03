#include <Signals/Signals.hpp>

void Signal::connect(std::string type, Callback cb) {
  std::lock_guard<std::mutex> lock(mutex_);
  {
    callbacks_.insert({type, cb});
  }
}

void Signal::emit(std::string type) {
  std::lock_guard<std::mutex> lock(mutex_);
  {
    if (callbacks_.contains(type)) {
      callbacks_.at(type)();
    }
  }
}

void Signal::disconnectAll() { callbacks_.clear(); }
