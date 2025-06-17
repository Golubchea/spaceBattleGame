#pragma once

#include <cstdint>
#include <string>
#include <vector>

class IObject {
public:
    virtual ~IObject() = default;
    virtual void SetProperty(const std::string& key, const std::vector<uint8_t>& value) = 0;
    virtual std::vector<uint8_t> GetProperty(const std::string& key) const = 0;
    virtual bool HasProperty(const std::string& key) const = 0;
};
