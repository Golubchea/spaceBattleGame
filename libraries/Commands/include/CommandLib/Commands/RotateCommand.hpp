#pragma once

#include "../ICommand.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <sstream>

class RotateCommand : public ICommand {
public:
    explicit RotateCommand(float angle);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;

    std::string Type() const override;

private:
    float angle_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(angle_);
    }
};

namespace {
bool registeredRotate = []() -> bool {
  ICommand::RegisterCommand(
      "RotateCommand", [](const uint8_t *data, size_t size, IObject *context) {
        std::istringstream iss(
            std::string(reinterpret_cast<const char *>(data), size));
        boost::archive::binary_iarchive iar(iss);
        std::string type;
        iar >> type;

        if (type != "RotateCommand") {
          throw std::runtime_error("Invalid command type");
        }

        float delta;
        iar >> delta;

        return std::make_shared<RotateCommand>(delta);
      });
  return true;
}();
} // namespace
