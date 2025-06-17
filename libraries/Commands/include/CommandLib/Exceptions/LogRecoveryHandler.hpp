#pragma once

#include "IRecoveryHandler.hpp"

class LogRecoveryHandler : public IRecoveryHandler {
public:
    bool CanHandle(std::shared_ptr<ICommand> command, const std::exception& error) override;
    std::shared_ptr<ICommand> Handle(std::shared_ptr<ICommand> command,
                                    const std::exception& error) override;
};