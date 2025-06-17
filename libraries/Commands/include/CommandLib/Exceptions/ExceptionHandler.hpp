#pragma once

#include <CommandLib/Exceptions/IRecoveryHandler.hpp>
#include <mutex>
#include <vector>

class ExceptionHandler {
public:
    static ExceptionHandler& GetInstance();

    void RegisterHandler(std::shared_ptr<IRecoveryHandler> handler);
    std::shared_ptr<ICommand> Handle(std::shared_ptr<ICommand> cmd, const std::exception& e);

private:
    ExceptionHandler() = default;
    std::vector<std::shared_ptr<IRecoveryHandler>> handlers_;
    std::mutex mtx_;
};
