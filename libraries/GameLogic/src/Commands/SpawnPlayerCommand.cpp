#include <ECSEngineLib/Components/FuelComponent.hpp>
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/VelocityComponent.hpp>
#include <ECSEngineLib/World.hpp>
#include <ServerLogic/Commands/SpawnPlayerCommand.hpp>
#include <ServerLogic/PlayerFactory.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <iostream>
#include <sstream>

// ==== Реализация ====
SpawnPlayerCommand::SpawnPlayerCommand(glm::vec3 position, glm::vec3 velocity,
                                       float angle, float fuel, int missiles)
    : position_(position), velocity_(velocity), angle_(angle), fuel_(fuel), missiles_(missiles) {}

void SpawnPlayerCommand::Execute(IObject* context) {
    auto world = dynamic_cast<World*>(context);
    if (!world) {
        std::cerr << "[SpawnPlayerCommand] Invalid context: expected World" << std::endl;
        return;
    }

    auto& container = IoCContainer::GetInstance();
    auto playerFactory =
        container.Resolve<std::shared_ptr<IPlayerFactory>>("PlayerFactory");

    // Создаём игрока
    auto player = playerFactory->CreatePlayer(
        {position_.x, position_.y, position_.z},
        {velocity_.x, velocity_.y, velocity_.z},
        angle_,
        fuel_,
        missiles_
    );

    std::cout << "[SpawnPlayerCommand] Player created with Entity ID: " << player->entity() << std::endl;
}

std::vector<uint8_t> SpawnPlayerCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    oa << *this;

    return {oss.str().begin(), oss.str().end()};
}

std::string SpawnPlayerCommand::Type() const {
    return "SpawnPlayerCommand";
}

template<class Archive>
void SpawnPlayerCommand::serialize(Archive& ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_NVP(position_);
    ar & BOOST_SERIALIZATION_NVP(velocity_);
    ar & BOOST_SERIALIZATION_NVP(angle_);
    ar & BOOST_SERIALIZATION_NVP(fuel_);
    ar & BOOST_SERIALIZATION_NVP(missiles_);
}

bool registeredSpawn = []() {
  ICommand::RegisterCommand(
      "SpawnPlayerCommand",
      [](const uint8_t *data, size_t size, IObject *context) {
        std::istringstream iss(
            std::string(reinterpret_cast<const char *>(data), size));
        boost::archive::binary_iarchive iar(iss);
        std::string type;
        iar >> type;

        if (type != "SpawnPlayerCommand") {
          throw std::runtime_error("Invalid command type");
        }

        SpawnPlayerCommand cmd;
        iar >> cmd;
        return std::make_shared<SpawnPlayerCommand>(cmd);
      });
  return true;
}();
