#include <CommandLib/Exceptions/CommandException.hpp>

CommandException::CommandException(const std::string& msg)
    : std::runtime_error(msg) {}
