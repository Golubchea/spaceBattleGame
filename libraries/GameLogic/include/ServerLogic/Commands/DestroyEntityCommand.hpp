#pragma once
#include <CommandLib/ICommand.hpp>
#include <ECSEngineLib/Entity.hpp>
#include <ECSEngineLib/World.hpp>
#include <boost/serialization/access.hpp>
class DestroyEntityCommand : public ICommand {
public:
  explicit DestroyEntityCommand(Entity entity, std::shared_ptr<World> wrld);
  void Execute(IObject *context) override;
  std::vector<uint8_t> Serialize() const override;
  std::string Type() const override;

private:
    Entity entity_;
    std::shared_ptr<World> world_;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version);
};
