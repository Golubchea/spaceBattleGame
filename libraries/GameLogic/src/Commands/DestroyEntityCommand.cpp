#include <CommandLib/ICommand.hpp>
#include <ECSEngineLib/World.hpp>
#include <ServerLogic/Commands/DestroyEntityCommand.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <sstream>

DestroyEntityCommand::DestroyEntityCommand(Entity entity,
                                           std::shared_ptr<World> wrld)
    : entity_(entity), world_(wrld) {}

void DestroyEntityCommand::Execute(IObject* context) {
  world_->DestroyEntity(entity_);
  std::cout << "[DestroyEntityCommand] Entity " << entity_ << " destroyed."
            << std::endl;
}

std::vector<uint8_t> DestroyEntityCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    oa << entity_;

    return {oss.str().begin(), oss.str().end()};
}

std::string DestroyEntityCommand::Type() const {
    return "DestroyEntityCommand";
}

template<class Archive>
void DestroyEntityCommand::serialize(Archive& ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_NVP(entity_);
}

bool registeredDestroy = []() {
  ICommand::RegisterCommand(
      "DestroyEntityCommand",
      [](const uint8_t *data, size_t size, IObject *context) {
        std::istringstream iss(
            std::string(reinterpret_cast<const char *>(data), size));
        boost::archive::binary_iarchive iar(iss);
        std::string type;
        iar >> type;

        if (type != "DestroyEntityCommand") {
          throw std::runtime_error("Invalid command type");
        }
        Entity entity;
        std::shared_ptr<World> wrld;
        DestroyEntityCommand cmd(entity, wrld);
        iar >> cmd;
        return std::make_shared<DestroyEntityCommand>(cmd);
      });
  return true;
}();
