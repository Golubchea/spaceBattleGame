#pragma once

#include "../Component.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

struct FuelComponent : public Component {

public:
  FuelComponent() = default;
  explicit FuelComponent(double f) : fuel(f) {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &boost::serialization::base_object<Component>(*this);
    ar & fuel;
  }

  double getFuel() const;
  void setFuel(double newFuel);

  friend std::ostream &operator<<(std::ostream &os, const FuelComponent &comp) {
    os << "Fuel: (" << comp.getFuel() << ")";
    return os;
  }
  void print(std::ostream &os) override { os << *this << "\n"; }

private:
  friend class boost::serialization::access;
  double fuel = 0.0;
};

inline double FuelComponent::getFuel() const { return fuel; }
inline void FuelComponent::setFuel(double newFuel) { fuel = newFuel; }

// BOOST_CLASS_EXPORT_GUID(FuelComponent, "FuelComponent")
