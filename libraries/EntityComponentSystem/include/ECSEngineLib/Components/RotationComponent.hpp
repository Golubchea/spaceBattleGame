#pragma once

#include "../Component.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

struct RotationComponent : public Component {

public:
  RotationComponent() = default;
  explicit RotationComponent(float f) : angle_(f) {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar & angle_;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const RotationComponent &comp) {
    os << "Rotation: (" << comp.angle() << ")";
    return os;
  }
  void print(std::ostream &os) override { os << *this << "\n"; }

  float angle() const;
  void setAngle(float newAngle);

private:
  friend class boost::serialization::access;
  float angle_ = 0.0;
};

inline float RotationComponent::angle() const { return angle_; }

inline void RotationComponent::setAngle(float newAngle) { angle_ = newAngle; }

// BOOST_CLASS_EXPORT_GUID(RotationComponent, "RotationComponent")
