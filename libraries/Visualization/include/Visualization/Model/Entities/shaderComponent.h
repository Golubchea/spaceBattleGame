#pragma once

#include "glm/ext.hpp"
#include <ECSEngineLib/Component.hpp>
#include <Visualization/Model/Entity.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/serialization.hpp>
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/string_cast.hpp>
class ShaderComponent : public Component {
public:
  ShaderComponent() = default;

  explicit ShaderComponent(std::shared_ptr<Shader> model)
      : shader_(std::move(model)) {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar &(shader_->getID());
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const ShaderComponent &comp) {
    os << "Shader: (" << std::to_string(comp.shader_->getID()) << ")";
    return os;
  }
  void print(std::ostream &os) override { os << *this << "\n"; }

  std::shared_ptr<Shader> shader() const;
  void setShader(const std::shared_ptr<Shader> &newShader);

private:
  std::shared_ptr<Shader> shader_;
  friend class boost::serialization::access;
};

// #include <boost/serialization/export.hpp>
// BOOST_CLASS_EXPORT_GUID(VelocityComponent, "VelocityComponent")

inline std::shared_ptr<Shader> ShaderComponent::shader() const {
  return shader_;
}

inline void
ShaderComponent::setShader(const std::shared_ptr<Shader> &newShader) {
  shader_ = newShader;
}
