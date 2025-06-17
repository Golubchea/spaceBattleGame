#pragma once

#include "../ICommand.hpp"
#include <array>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
#include <sstream>
class MoveCommand : public ICommand {
public:
    explicit MoveCommand(const std::array<double, 2> &delta);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;

    std::string Type() const override;

    void setDelta(const std::array<double, 2> &newDelta);

  private:
    std::array<double, 2> delta_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
      ar &Type();
      ar &BOOST_SERIALIZATION_NVP(delta_);
    }
};

namespace {
bool registeredMove = []() -> bool {
  ICommand::RegisterCommand(
      "MoveCommand", [](const uint8_t *data, size_t size, IObject *context) {
        std::string buffer(reinterpret_cast<const char *>(data), size);
        std::istringstream iss(buffer);
        boost::archive::binary_iarchive ia(iss);

        std::string type;
        ia >> type;

        if (type != "MoveCommand") {
          throw std::runtime_error("Invalid command type");
        }

        std::array<double, 2> delta;
        ia >> delta;

        return std::make_shared<MoveCommand>(delta);
      });
  return true;
}();
} // namespace
