#include <CommandLib/Commands/LogExceptionCommand.hpp>
#include <iostream>

LogExceptionCommand::LogExceptionCommand(const std::string& msg) : message_(msg) {}

void LogExceptionCommand::Execute(IObject* object) {
    std::cerr << "[LOG] " << message_ << std::endl;
}

std::vector<uint8_t> LogExceptionCommand::Serialize() const
{
    return {};
}

std::string LogExceptionCommand::Type() const
{
    return "LogExceptionCommand";
}
