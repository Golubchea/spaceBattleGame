#pragma once

#include "../ICommand.hpp"
#include <array>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>

class ChangeVelocityCommand : public ICommand {
public:
    explicit ChangeVelocityCommand(const std::array<double, 2> &delta);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;

    std::string Type() const override;

private:
    std::array<double, 2> delta_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(delta_);
    }
};
