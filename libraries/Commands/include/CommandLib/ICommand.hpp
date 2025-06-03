#pragma once

#include "IObject.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <boost/serialization/export.hpp>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute(IObject* object) = 0;
    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual std::string Type() const = 0;
    using Deserializer = std::function<std::shared_ptr<ICommand>(const uint8_t*, size_t, IObject*)>;

    static void RegisterCommand(const std::string& type, Deserializer deserializer);
    static std::shared_ptr<ICommand> Deserialize(const uint8_t* data, size_t size, IObject* context);

private:
    static std::unordered_map<std::string, Deserializer>& getRegistry();
    static std::mutex& getMutex();
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICommand)
