#include <CommandLib/Exceptions/NotEnoughFuelException.hpp>

NotEnoughFuelException::NotEnoughFuelException(const std::string& msg)
    : CommandException(msg) {}
