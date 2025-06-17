#include <CommandLib/Exceptions/ExceptionHandler.hpp>
#include <iostream>

ExceptionHandler& ExceptionHandler::GetInstance() {
    static ExceptionHandler instance;
    return instance;
}

void ExceptionHandler::RegisterHandler(std::shared_ptr<IRecoveryHandler> handler) {
    std::lock_guard<std::mutex> lock(mtx_);
    handlers_.push_back(handler);
}

std::shared_ptr<ICommand> ExceptionHandler::Handle(
    std::shared_ptr<ICommand> command,
    const std::exception& error)
{
    std::lock_guard<std::mutex> lock(mtx_);

    for (auto& handler : handlers_) {
        if (handler->CanHandle(command, error)) {
            return handler->Handle(command, error);
        }
    }

    std::cerr << "[ExceptionHandler] No recovery handler found" << std::endl;
    return nullptr;
}
