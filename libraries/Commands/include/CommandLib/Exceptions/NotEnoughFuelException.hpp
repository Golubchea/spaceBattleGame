#pragma once

#include "CommandException.hpp"

class NotEnoughFuelException : public CommandException {
public:
    explicit NotEnoughFuelException(const std::string& msg);
};