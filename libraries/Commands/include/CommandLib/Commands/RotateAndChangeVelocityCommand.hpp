#pragma once

#include "../ICommand.hpp"
#include <CommandLib/Commands/BurnFuelCommand.hpp>
#include <CommandLib/Commands/ChangeVelocityCommand.hpp>
#include <CommandLib/Commands/CheckFuelCommand.hpp>
#include <CommandLib/Commands/MacroCommand.hpp>
#include <CommandLib/Commands/MoveCommand.hpp>
#include <CommandLib/Commands/RotateCommand.hpp>
#include <array>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
#include <sstream>

class RotateAndChangeVelocityCommand : public MacroCommand
{
public:
    explicit RotateAndChangeVelocityCommand(float deltaRotate,
                                            const std::array<double, 2> &deltaVelocity)
    {
        Add(std::make_shared<RotateCommand>(deltaRotate));
        Add(std::make_shared<ChangeVelocityCommand>(deltaVelocity));
        //Add(std::make_shared<BurnFuelCommand>(1.0f)); // расход топлива за шаг
    }
};
