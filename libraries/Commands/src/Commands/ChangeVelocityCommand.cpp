#include <CommandLib/Commands/ChangeVelocityCommand.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <sstream>

ChangeVelocityCommand::ChangeVelocityCommand(const std::array<double, 2> &delta)
    : delta_(delta)
{}

void ChangeVelocityCommand::Execute(IObject* object) {
    if (!object->HasProperty("velocity")) return;

    auto velData = object->GetProperty("velocity");
    double* velocity = reinterpret_cast<double*>(velData.data());
    velocity[0] += delta_[0];
    velocity[1] += delta_[1];
    object->SetProperty("velocity", std::vector<uint8_t>(
        reinterpret_cast<uint8_t*>(velocity), reinterpret_cast<uint8_t*>(velocity + 2)));
}

std::vector<uint8_t> ChangeVelocityCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    oa << *this;
    const std::string &serialized = oss.str(); // <-- Сохраняем один раз
    return std::vector<uint8_t>(serialized.begin(), serialized.end());
}

std::string ChangeVelocityCommand::Type() const {
    return "ChangeVelocityCommand";
}

namespace {
bool registered = []() -> bool {
    ICommand::RegisterCommand("ChangeVelocityCommand",
                              [](const uint8_t *data, size_t size, IObject *context) {
                                  std::istringstream iss(
                                      std::string(reinterpret_cast<const char *>(data), size));
                                  boost::archive::binary_iarchive iar(iss);
                                  std::string type;
                                  iar >> type;
                                  ChangeVelocityCommand cmd(std::array<double, 2>{0, 0});
                                  iar >> cmd;
                                  return std::make_shared<ChangeVelocityCommand>(cmd);
                              });
    return true;
}();
}
