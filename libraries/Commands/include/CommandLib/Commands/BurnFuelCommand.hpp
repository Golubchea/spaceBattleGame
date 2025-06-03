#pragma once

#include "../ICommand.hpp"
#include <boost/serialization/nvp.hpp>

class BurnFuelCommand : public ICommand {
public:
    explicit BurnFuelCommand(float amount);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;

    std::string Type() const override;

private:
    float amount_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(amount_);
    }
};
