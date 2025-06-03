
#include <ECSEngineLib/World.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "ECSEngineLib/Components/FuelComponent.hpp"
#include "ECSEngineLib/Components/MissleComponent.hpp"
#include "ECSEngineLib/Components/PositionComponent.hpp"
#include "ECSEngineLib/Components/VelocityComponent.hpp"
#include <ECSEngineLib/World.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>

World::World()
    : next_entity_id_(1)
{}

Entity World::CreateEntity() {
    return next_entity_id_++;
}

void World::DestroyEntity(Entity entity) {
  for (auto &[type, map] : components_) {
    map.erase(entity);
  }
}

void World::Save(const std::string &filename) {
  std::ofstream ofs(filename, std::ios::binary);
  boost::archive::binary_oarchive oa(ofs);
  oa << *this;
}

void World::Load(const std::string &filename) {
  std::ifstream ifs(filename, std::ios::binary);
  boost::archive::binary_iarchive ia(ifs);
  ia >> *this;
}

std::unordered_map<std::string,
                   std::unordered_map<Entity, std::shared_ptr<Component>>>
World::components() const {
  return components_;
}

void World::setComponents(
    const std::unordered_map<
        std::string, std::unordered_map<Entity, std::shared_ptr<Component>>>
        &newComponents) {
  components_ = newComponents;
}

BOOST_CLASS_EXPORT_GUID(PositionComponent, "PositionComponent")
BOOST_CLASS_EXPORT_GUID(VelocityComponent, "VelocityComponent")
BOOST_CLASS_EXPORT_GUID(FuelComponent, "FuelComponent")
BOOST_CLASS_EXPORT_GUID(MissleComponent, "MissleComponent")
