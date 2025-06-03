#pragma once
#include <boost/serialization/array.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <ostream>

class Component {

public:
  virtual ~Component() = default;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    // Базовые поля, если есть
    // ar &boost::serialization::base_object<Component>(*this);
  }
  virtual void print(std::ostream &os) = 0;
};

#include <boost/serialization/export.hpp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Component)
