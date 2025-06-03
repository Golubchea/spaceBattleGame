#pragma once

#include "../ICommand.hpp"
#include <CommandLib/Commands/SharedICommandSerialization.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <sstream>
#include <vector>

class MacroCommand : public ICommand {
public:
    void Add(std::shared_ptr<ICommand> command);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;
    std::string Type() const override;

    std::vector<std::shared_ptr<ICommand>> GetCommands() const;

private:
    std::vector<std::shared_ptr<ICommand>> commands_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(commands_);
    }
};

namespace {
bool registeredMacro = []() -> bool {
  ICommand::RegisterCommand(
      "MacroCommand", [](const uint8_t *data, size_t size, IObject *context) {
        std::istringstream iss(
            std::string(reinterpret_cast<const char *>(data), size));
        boost::archive::binary_iarchive iar(iss);
        std::string type;
        iar >> type;
        MacroCommand cmd;
        iar >> cmd;
        return std::make_shared<MacroCommand>(cmd);
      });
  return true;
}();
} // namespace
