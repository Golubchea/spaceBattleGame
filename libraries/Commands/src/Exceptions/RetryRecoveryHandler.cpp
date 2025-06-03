#include <CommandLib/Commands/RetryCommand.hpp>
#include <CommandLib/Exceptions/RetryRecoveryHandler.hpp>
#include <iostream>

bool RetryRecoveryHandler::CanHandle(std::shared_ptr<ICommand> command,
                                   const std::exception& error) {
    return true; // попробуем повторить любую команду
}

std::shared_ptr<ICommand> RetryRecoveryHandler::Handle(
    std::shared_ptr<ICommand> command,
    const std::exception& error)
{
    std::cerr << "[RetryRecoveryHandler] Retrying command..." << std::endl;
    return std::make_shared<RetryCommand>(command);
}
