// ECSBasedPlayer.cpp
#include "ServerLogic/Player.hpp"
#include <ECSEngineLib/Components/PositionComponent.hpp>
#include <ECSEngineLib/Components/VelocityComponent.hpp>

Player::Player(Entity entity, std::shared_ptr<World> world)
    : entity_(entity), world_(world) {}

void Player::SetProperty(const std::string &key,
                         const std::vector<uint8_t> &value) {
  if (key == "position") {
    auto *arr = reinterpret_cast<const std::array<double, 2> *>(value.data());
    auto comp = world_->GetComponent<PositionComponent>(entity_);
    if (!comp) {
      world_->AddComponent<PositionComponent>(
          entity_, PositionComponent{{(*arr)[0], (*arr)[1]}});
    } else {
      comp->setPosition({(*arr)[0], (*arr)[1]});
    }
  }
}

std::vector<uint8_t> Player::GetProperty(const std::string &key) const {
  std::vector<uint8_t> result;
  if (key == "position") {
    auto comp = world_->GetComponent<PositionComponent>(entity_);
    if (comp) {
      std::array<double, 2> pos = {comp->getPosition()[0],
                                   comp->getPosition()[1]};
      result.resize(sizeof(pos));
      std::memcpy(result.data(), &pos, sizeof(pos));
    }
  }
  return result;
}

bool Player::HasProperty(const std::string &key) const {
  if (key == "position") {
    return world_->HasComponent<PositionComponent>(entity_);
  }
  return false;
}

void Player::Clear() { /*world_.DestroyEntity(entity_);*/ }

Entity Player::entity() const { return entity_; }

void Player::setEntity(Entity newEntity) { entity_ = newEntity; }

Vector<double, 2> Player::getPosition() const {
  auto comp = world_->GetComponent<PositionComponent>(entity_);
  if (!comp)
    return {0, 0};
  return {comp->getPosition()[0], comp->getPosition()[1]};
}

Vector<double, 2> Player::getVelocity() const {
  auto comp = world_->GetComponent<VelocityComponent>(entity_);
  if (!comp)
    return {0, 0};
  return {comp->getVelocity()[0], comp->getVelocity()[1]};
}

void Player::setPosition(const Vector<double, 2> &newV) {
  auto comp = world_->GetComponent<PositionComponent>(entity_);
  if (!comp) {
    world_->AddComponent<PositionComponent>(
        entity_, PositionComponent{{newV.x(), newV.y()}});
  } else {
    comp->setPosition({newV.x(), newV.y()});
  }
}
