#pragma once
#include "Component.hpp"
#include <memory>

class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::shared_ptr<Component> Create() const = 0;
};
