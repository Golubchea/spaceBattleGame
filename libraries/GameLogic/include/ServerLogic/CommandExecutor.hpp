#pragma once

#include "ServerLogic/CommandService.hpp"
#include <CommandLib/CommandQueue.hpp>
#include <CommandLib/ICommand.hpp>
#include <ServerLogic/Player.hpp>
#include <functional>
// #include <mutex>
// #include <thread>

class CommandExecutor : public CommandService {
public:
  using CommandWithContext =
      std::pair<std::shared_ptr<ICommand>, std::shared_ptr<IObject>>;

  explicit CommandExecutor(std::function<void()> updateCallback);
  void EnqueueCommand(std::shared_ptr<ICommand> command,
                      std::shared_ptr<IObject> context) override;
  void StartProcessing() override;
  void Stop();
  ~CommandExecutor();

  void WorkerLoop() override;

private:

  CommandQueue queue_;
  // std::thread worker_;
  bool stopped_ = false;
  std::function<void()> updateCallback_;
  // mutable std::mutex mtx_;
};
