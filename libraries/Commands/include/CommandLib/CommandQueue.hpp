#pragma once

#include "ICommand.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>

struct CommandQueueItem {
  std::shared_ptr<ICommand> command;
  std::shared_ptr<IObject> context;
};

class CommandQueue {
public:
  void Push(std::shared_ptr<ICommand> command,
            std::shared_ptr<IObject> context);
  CommandQueueItem Pop();
  bool Empty() const;

  void Stop();
  bool IsStopped() const;

private:
  std::queue<CommandQueueItem> queue_;
  mutable std::mutex mtx_;
  std::condition_variable cv_;
  bool stopped_ = false;
};
