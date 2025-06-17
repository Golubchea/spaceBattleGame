#pragma once

#include <ECSEngineLib/Component.hpp>
#include <Visualization/Model/Entity.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/serialization.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
class ModelComponent : public Component {
public:
  ModelComponent() = default;

  // explicit ModelComponent(std::shared_ptr<Object3d> model) : model_(model) {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar & "model";
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const ModelComponent &comp) {
    os << "ModelMatrix: (" << glm::to_string(comp.getModel()->getPosition())
       << ")";
    return os;
  }
  void print(std::ostream &os) override { os << *this << "\n"; }

  std::shared_ptr<Object3d> getModel() const;
  void setModel(const std::shared_ptr<Object3d> &newModel);

private:
  std::shared_ptr<Object3d> model_ = nullptr;
  friend class boost::serialization::access;
};

inline std::shared_ptr<Object3d> ModelComponent::getModel() const {
  return model_;
}

inline void
ModelComponent::setModel(const std::shared_ptr<Object3d> &newModel) {
  model_ = newModel;
}
