#pragma once
#include "ServerLogic/Math/Vector.h"
#include <CommandLib/IObject.hpp>
#include <ECSEngineLib/Entity.hpp>
#include <ECSEngineLib/World.hpp>

#include <string>
#include <vector>

class Player : public IObject {
public:
  explicit Player(Entity entity, std::shared_ptr<World> world);

  // === IObject ===
  void SetProperty(const std::string &key,
                   const std::vector<uint8_t> &value) override;
  std::vector<uint8_t> GetProperty(const std::string &key) const override;
  bool HasProperty(const std::string &key) const override;

  // === IMovingObject ===
  Vector<double, 2> getPosition() const;
  Vector<double, 2> getVelocity() const;
  void setPosition(const Vector<double, 2> &newV);

  void Clear();

  Entity entity() const;
  void setEntity(Entity newEntity);

private:
  Entity entity_;
  std::shared_ptr<World> world_;
};
