#include <CommandLib/Commands/LogExceptionCommand.hpp>
#include <CommandLib/Commands/RetryCommand.hpp>
#include <CommandLib/Exceptions/TwoTriesThenLogHandler.hpp>
#include <iostream>

TwoTriesThenLogHandler::TwoTriesThenLogHandler() = default;

bool TwoTriesThenLogHandler::CanHandle(std::shared_ptr<ICommand> command,
                                      const std::exception& error) {
    std::lock_guard<std::mutex> lock(mtx_);
    return true;
}

std::shared_ptr<ICommand> TwoTriesThenLogHandler::Handle(
    std::shared_ptr<ICommand> command,
    const std::exception& error)
{
    std::lock_guard<std::mutex> lock(mtx_);
    int count = retry_count_[command]++;
    if (count < 2) {
        std::cerr << "[TwoTriesThenLogHandler] Retrying (" << count + 1 << "/2)" << std::endl;
        return std::make_shared<RetryCommand>(command);
    } else {
        std::cerr << "[TwoTriesThenLogHandler] Giving up. Logging error." << std::endl;
        retry_count_.erase(command);
        return std::make_shared<LogExceptionCommand>("Failed after 2 retries: " + std::string(error.what()));
    }
}
