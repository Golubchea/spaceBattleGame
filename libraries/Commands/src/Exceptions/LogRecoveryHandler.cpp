#include <CommandLib/Commands/LogExceptionCommand.hpp>
#include <CommandLib/Exceptions/LogRecoveryHandler.hpp>
#include <iostream>

bool LogRecoveryHandler::CanHandle(std::shared_ptr<ICommand> command,
                                  const std::exception& error) {
    return true; // всегда можно залогировать
}

std::shared_ptr<ICommand> LogRecoveryHandler::Handle(std::shared_ptr<ICommand> command,
                                                    const std::exception& error) {
    std::cerr << "[LogRecoveryHandler] Logging exception: " << error.what() << std::endl;
    return std::make_shared<LogExceptionCommand>(error.what());
}
