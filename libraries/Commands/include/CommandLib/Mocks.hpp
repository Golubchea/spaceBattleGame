#pragma once

#include <CommandLib/IObject.hpp>
#include <string>
#include <unordered_map>
#include <vector>

// === Mock реализация IObject для тестов ===
class MockIObject : public IObject {
public:
    void SetProperty(const std::string& key, const std::vector<uint8_t>& value) override {
        properties_[key] = value;
    }

    std::vector<uint8_t> GetProperty(const std::string& key) const override {
        auto it = properties_.find(key);
        if (it == properties_.end()) return {};
        return it->second;
    }

    bool HasProperty(const std::string& key) const override {
        return properties_.find(key) != properties_.end();
    }

    void Clear() {
        properties_.clear();
    }

private:
    std::unordered_map<std::string, std::vector<uint8_t>> properties_;
};