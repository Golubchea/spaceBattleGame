#pragma once

#include "../Component.hpp"
#include <boost/serialization/array.hpp>
#include <boost/serialization/serialization.hpp>
class MissleComponent : public Component {

public:
  MissleComponent() = default;
  explicit MissleComponent(int missles) : missles_(missles) {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar & missles_;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const MissleComponent &comp) {
    os << "Missles: (" << comp.getMissles() << ")";
    return os;
  }
  void print(std::ostream &os) override { os << *this << "\n"; }

  int getMissles() const;
  void setMissles(int newMissles);

private:
  int missles_;
  friend class boost::serialization::access;
};

// #include <boost/serialization/export.hpp>
// BOOST_CLASS_EXPORT_GUID(VelocityComponent, "VelocityComponent")

inline int MissleComponent::getMissles() const { return missles_; }

inline void MissleComponent::setMissles(int newMissles) {
  missles_ = newMissles;
}
