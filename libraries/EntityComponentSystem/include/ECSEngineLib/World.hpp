#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include <boost/core/demangle.hpp>
#include <boost/core/demangle.hpp> // для typeid().name(), можно опустить если не нужно
#include <boost/serialization/array.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
// Сериализация std::unordered_map
namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, std::type_index &t, unsigned int) {
  std::string name = t.name();
  ar & name;
  // Note: точное восстановление type_index невозможно без RTTI или
  // дополнительной регистрации
}

} // namespace serialization
} // namespace boost

class World {
public:
  using UpdateCallback = std::function<void(float)>;

  void RegisterSystem(const UpdateCallback &callback) {
    updateCallbacks_.push_back(callback);
  }

  World();

  Entity CreateEntity();
  void DestroyEntity(Entity entity);

  template <typename T, typename... Args>
  void AddComponent(Entity entity, Args &&...args) {
    auto comp = std::make_shared<T>(std::forward<Args>(args)...);
    components_[typeid(T).name()][entity] = comp;
  }

  std::vector<Entity> getAllEntities() const {
    std::vector<Entity> result;
    for (const auto &[typeName, entityMap] : components_) {
      for (const auto &[entity, component] : entityMap) {
        result.push_back(entity);
      }
    }
    return result;
  }

  template <typename T> bool HasComponent(Entity entity) const {
    auto it = components_.find(typeid(T).name());
    if (it == components_.end())
      return false;
    return it->second.find(entity) != it->second.end();
  }

    template <typename T> std::shared_ptr<T> GetComponent(Entity entity) const {
      auto it = components_.find(typeid(T).name());
      if (it == components_.end())
        return nullptr;
      auto compIt = it->second.find(entity);
      if (compIt == it->second.end())
        return nullptr;
      return std::static_pointer_cast<T>(compIt->second);
    }

    void Save(const std::string &filename);
    void Load(const std::string &filename);
    void Update(float dt) {
      for (const auto &callback : updateCallbacks_) {
        callback(dt);
      }
    };

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
      ar & next_entity_id_;
      ar & components_;
    }

    friend std::ostream &operator<<(std::ostream &os, const World &world) {
      std::unordered_map<Entity, std::vector<std::shared_ptr<Component>>>
          entityComponents;

      // Собираем все компоненты для каждой сущности
      for (const auto &[typeName, entityMap] : world.components_) {
        for (const auto &[entity, component] : entityMap) {
          entityComponents[entity].push_back(component);
        }
      }

      if (entityComponents.empty()) {
        os << "World is empty.\n";
        return os;
      }

      os << "World contents:\n";
      for (const auto &[entity, components] : entityComponents) {
        os << "Entity " << entity << " has components:\n";
        for (const auto &component : components) {
          component->print(os);
          // os << component.get() << "\n";
        }
      }

      return os;
    }

    // template <typename T> std::vector<Entity> getAllEntitiesWith() const {
    //   std::vector<Entity> result;

    //   auto it = components_.find(typeid(T).name());
    //   if (it != components_.end()) {
    //     const auto &entityMap = it->second;
    //     result.reserve(entityMap.size());

    //     for (const auto &[entity, component] : entityMap) {
    //       result.push_back(entity);
    //     }
    //   }

    //   return result;
    // }

    template <typename T> std::vector<Entity> getAllEntitiesWith() const {
      std::vector<Entity> result;

      try {
        auto it = components_.find(typeid(T).name());
        if (it == components_.end()) {
          return result; // ранний выход, если тип не найден
        }

        const auto &entityMap = it->second;
        if (entityMap.empty()) {
          return result; // ещё один уровень защиты
        }

        result.reserve(entityMap.size());
        for (const auto &[entity, component] : entityMap) {
          if (component) { // Дополнительно: проверяем, что компонент не nullptr
            result.push_back(entity);
          }
        }

      } catch (const std::exception &ex) {
        std::cerr << "[ERROR] Exception in getAllEntitiesWith<"
                  << typeid(T).name() << ">: " << ex.what() << std::endl;
      } catch (...) {
        std::cerr << "[ERROR] Unknown exception in getAllEntitiesWith<"
                  << typeid(T).name() << ">" << std::endl;
      }

      return result;
    }

    std::unordered_map<std::string,
                       std::unordered_map<Entity, std::shared_ptr<Component>>>
    components() const;
    void setComponents(
        const std::unordered_map<
            std::string, std::unordered_map<Entity, std::shared_ptr<Component>>>
            &newComponents);

    void clear() {
      components_.clear();
      next_entity_id_ = 0;
    }

  private:
    std::vector<UpdateCallback> updateCallbacks_;
    Entity next_entity_id_;
    std::unordered_map<std::string,
                       std::unordered_map<Entity, std::shared_ptr<Component>>>
        components_;
};
