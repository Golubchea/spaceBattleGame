#include <CommandLib/Commands/CheckFuelCommand.hpp>
#include <CommandLib/Exceptions/NotEnoughFuelException.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <iostream>
#include <sstream>

void CheckFuelCommand::Execute(IObject* object) {
    if (!object->HasProperty("fuel")) {
        throw CommandException("No fuel component");
    }

    auto fuelData = object->GetProperty("fuel");
    float *fuel = reinterpret_cast<float *>(fuelData.data());

    if (*fuel <= 1.0f) {
        throw NotEnoughFuelException("Not enough fuel to move");
    }
}

std::vector<uint8_t> CheckFuelCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    return {oss.str().begin(), oss.str().end()};
}

std::string CheckFuelCommand::Type() const {
    return "CheckFuelCommand";
}

namespace {
bool registered = []() -> bool {
    ICommand::RegisterCommand("CheckFuelCommand", [](const uint8_t* data, size_t size, IObject* context) {
        return std::make_shared<CheckFuelCommand>();
    });
    return true;
}();
}
