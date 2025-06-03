#pragma once

#include <CommandLib/ICommand.hpp>
#include <string>

class LogExceptionCommand : public ICommand {
public:
    explicit LogExceptionCommand(const std::string& msg);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;
    std::string Type() const override;

private:
    std::string message_;
};
