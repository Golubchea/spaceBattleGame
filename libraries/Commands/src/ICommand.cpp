#include <CommandLib/ICommand.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>
#include <sstream>
// Реализация реестра команд
std::unordered_map<std::string, ICommand::Deserializer> &ICommand::getRegistry()
{
    static std::unordered_map<std::string, ICommand::Deserializer> registry;
    return registry;
}

std::mutex &ICommand::getMutex()
{
    static std::mutex mtx;
    return mtx;
}

void ICommand::RegisterCommand(const std::string &type, Deserializer deserializer)
{
  // if (getRegistry().count(type) == 0) {
  std::lock_guard<std::mutex> lock(getMutex());
  getRegistry()[type] = std::move(deserializer);
  //}
}

std::shared_ptr<ICommand> ICommand::Deserialize(const uint8_t *data,
                                                size_t size, IObject *context) {
  if (!data || size == 0) {
    std::cerr << "Deserialize error: Empty data or zero size.\n";
    return nullptr;
  }

  try {
    std::istringstream iss(
        std::string(reinterpret_cast<const char *>(data), size));
    boost::archive::binary_iarchive iar(iss);

    std::string type;
    iar >> type;

    std::lock_guard<std::mutex> lock(getMutex());
    auto it = getRegistry().find(type);
    if (it == getRegistry().end()) {
      std::cerr << "Deserialize error: Unknown command type '" << type
                << "'.\n";
      return nullptr;
    }

    // Вызываем десериализатор для конкретного типа команды
    auto command = it->second(data, size, context);
    if (!command) {
      std::cerr << "Deserialize error: Failed to deserialize command of type '"
                << type << "'.\n";
    }

    return command;
  } catch (const std::exception &e) {
    std::cerr << "Deserialize exception: " << e.what() << "\n";
  } catch (...) {
    std::cerr << "Deserialize exception: Unknown error.\n";
  }

  return nullptr;
}
