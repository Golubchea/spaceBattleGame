// CommandExecutor.cpp
#include <CommandLib/Exceptions/ExceptionHandler.hpp>
#include <CommandLib/Exceptions/LogRecoveryHandler.hpp>
#include <CommandLib/Exceptions/NotEnoughFuelException.hpp>
#include <CommandLib/Exceptions/RetryRecoveryHandler.hpp>
#include <CommandLib/Exceptions/TwoTriesThenLogHandler.hpp>
#include <CommandLib/Mocks.hpp>
#include <ServerLogic/CommandExecutor.hpp>

CommandExecutor::CommandExecutor(std::function<void()> updateCallback)
    : updateCallback_(updateCallback) {

  ExceptionHandler::GetInstance().RegisterHandler(
      std::make_shared<LogRecoveryHandler>());
  ExceptionHandler::GetInstance().RegisterHandler(
      std::make_shared<RetryRecoveryHandler>());

  // worker_ = std::thread(&CommandExecutor::WorkerLoop, this);
}

void CommandExecutor::WorkerLoop() {
  // while (!stopped_) {
  auto item = queue_.Pop();
  if (!item.command)
    return;
  //  continue;

  try {
    item.command->Execute(item.context.get());
    if (updateCallback_)
      updateCallback_();
  } catch (const std::exception &e) {
    auto handlerCommand =
        ExceptionHandler::GetInstance().Handle(item.command, e);
    handlerCommand->Execute(item.context.get()); // повтор
  }
}
// }

void CommandExecutor::EnqueueCommand(std::shared_ptr<ICommand> command,
                                     std::shared_ptr<IObject> context) {
  queue_.Push(command, context);
}

void CommandExecutor::StartProcessing() {
  // Не используется, если поток запускается автоматически
}

CommandExecutor::~CommandExecutor() { Stop(); }

void CommandExecutor::Stop() {
  queue_.Stop();
  // if (worker_.joinable())
  //   worker_.join();
}
