#include <CommandLib/Commands/MoveCommand.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <sstream>

MoveCommand::MoveCommand(const std::array<double, 2> &delta) : delta_(delta) {}

void MoveCommand::Execute(IObject* object) {
    if (!object->HasProperty("position")) return;

    auto posData = object->GetProperty("position");
    double* position = reinterpret_cast<double*>(posData.data());
    position[0] += delta_[0];
    position[1] += delta_[1];

    object->SetProperty("position", std::vector<uint8_t>(
        reinterpret_cast<uint8_t*>(position), reinterpret_cast<uint8_t*>(position + 2)));
}
std::vector<uint8_t> MoveCommand::Serialize() const {
  std::ostringstream oss;
  boost::archive::binary_oarchive oa(oss);

  oa << Type();
  oa << delta_; // Явно указываем поле delta_

  std::string serialized = oss.str();
  return {serialized.begin(), serialized.end()};
}

std::string MoveCommand::Type() const { return "MoveCommand"; }

void MoveCommand::setDelta(const std::array<double, 2> &newDelta) {
  delta_ = newDelta;
}
