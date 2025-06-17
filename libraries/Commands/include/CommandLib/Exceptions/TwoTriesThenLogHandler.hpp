#pragma once

#include "IRecoveryHandler.hpp"
#include <unordered_map>
#include <atomic>

class TwoTriesThenLogHandler : public IRecoveryHandler {
public:
    TwoTriesThenLogHandler();

    bool CanHandle(std::shared_ptr<ICommand> command, const std::exception& error) override;
    std::shared_ptr<ICommand> Handle(std::shared_ptr<ICommand> command,
                                    const std::exception& error) override;

private:
    std::unordered_map<std::shared_ptr<ICommand>, int> retry_count_;
    std::mutex mtx_;
};