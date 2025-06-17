#pragma once

#include <CommandLib/ICommand.hpp>
#include <exception>

class IRecoveryHandler {
public:
    virtual ~IRecoveryHandler() = default;
    virtual bool CanHandle(std::shared_ptr<ICommand> command, const std::exception& error) = 0;
    virtual std::shared_ptr<ICommand> Handle(std::shared_ptr<ICommand> command,
                                             const std::exception& error) = 0;
};
