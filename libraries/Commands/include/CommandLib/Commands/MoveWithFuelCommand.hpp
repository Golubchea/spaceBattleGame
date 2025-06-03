#pragma once

#include "../ICommand.hpp"
#include <CommandLib/Commands/BurnFuelCommand.hpp>
#include <CommandLib/Commands/CheckFuelCommand.hpp>
#include <CommandLib/Commands/MacroCommand.hpp>
#include <CommandLib/Commands/MoveCommand.hpp>
#include <array>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>

class MoveWithFuelCommand : public MacroCommand
{
public:
    explicit MoveWithFuelCommand(const std::array<double, 2> &delta)
    {
        Add(std::make_shared<CheckFuelCommand>());
        Add(std::make_shared<MoveCommand>(delta));
        Add(std::make_shared<BurnFuelCommand>(1.0f)); // расход топлива за шаг
    }
};
