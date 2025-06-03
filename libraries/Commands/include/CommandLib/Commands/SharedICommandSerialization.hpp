#pragma once

#include <CommandLib/ICommand.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/vector.hpp>

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive &ar, std::shared_ptr<ICommand> &cmd, unsigned int version) {
    split_free(ar, cmd, version);
}

} // namespace serialization
} // namespace boost
