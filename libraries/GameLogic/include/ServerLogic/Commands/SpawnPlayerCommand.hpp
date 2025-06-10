#pragma once
#include <CommandLib/ICommand.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <glm/glm.hpp>
#include <string>

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, glm::vec3 &v, const unsigned int version) {
  ar & v.x;
  ar & v.y;
  ar & v.z;
}

} // namespace serialization
} // namespace boost

class SpawnPlayerCommand : public ICommand {
public:
    explicit SpawnPlayerCommand(glm::vec3 position = glm::vec3(0, 0, 0),
                                glm::vec3 velocity = glm::vec3(0, 0, 0),
                                float angle = 0.0f,
                                float fuel = 100.0f,
                                int missiles = 5);

    void Execute(IObject* context) override;
    std::vector<uint8_t> Serialize() const override;
    std::string Type() const override;

private:
    glm::vec3 position_;
    glm::vec3 velocity_;
    float angle_;
    float fuel_;
    int missiles_;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version);
};
