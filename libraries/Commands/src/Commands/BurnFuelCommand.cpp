#include <CommandLib/Commands/BurnFuelCommand.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <sstream>

BurnFuelCommand::BurnFuelCommand(float amount) : amount_(amount) {}

void BurnFuelCommand::Execute(IObject* object) {
    if (!object->HasProperty("fuel")) return;

    auto fuelData = object->GetProperty("fuel");
    float* fuel = reinterpret_cast<float*>(fuelData.data());
    *fuel = std::max(*fuel - amount_, 0.0f);
    object->SetProperty("fuel", std::vector<uint8_t>(
        reinterpret_cast<uint8_t*>(fuel),
        reinterpret_cast<uint8_t*>(fuel + 1)));
}

std::vector<uint8_t> BurnFuelCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    oa << *this;
    return {oss.str().begin(), oss.str().end()};
}

std::string BurnFuelCommand::Type() const {
    return "BurnFuelCommand";
}

namespace {
bool registered = []() -> bool {
    ICommand::RegisterCommand("BurnFuelCommand", [](const uint8_t* data, size_t size, IObject* context) {
        std::istringstream iss(std::string(reinterpret_cast<const char*>(data), size));
        boost::archive::binary_iarchive iar(iss);
        std::string type;
        
        iar >> type;
        BurnFuelCommand cmd(0.0f);
        iar >> cmd;
        return std::make_shared<BurnFuelCommand>(cmd);
    });
    return true;
}();
}
