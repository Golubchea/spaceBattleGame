#pragma once
#include <CommandLib/ICommand.hpp>
#include <InversionOfControl/InversionOfControl.h>

class CommandService {
public:
  virtual ~CommandService() = default;
  virtual void EnqueueCommand(std::shared_ptr<ICommand> command,
                              std::shared_ptr<IObject> context) = 0;
  virtual void StartProcessing() = 0;

  virtual void WorkerLoop() = 0;
};
