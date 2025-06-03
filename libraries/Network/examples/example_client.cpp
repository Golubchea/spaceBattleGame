
#include <crashHandler/stacktrace.hpp>
#include <iostream>
#include <network/NetworkFactory.hpp>
#include <utils/conversionUtils.hpp>
#include <vector>

namespace {
constexpr char request[] = "clientToServer";
constexpr char address[] = "127.0.0.1";
int port = 8080;

} // namespace

int main(int argc, char *argv[]) {

  auto crashHandler = StackTraceFactory::createStackTrace();

  crashHandler->init([]() {
    auto client = network::NetworkFactory::createClient();
    client->setOnConnect([&]() {
      std::cout << "Connected to server\n";
      for (auto i = 0u; i < 100; ++i) {
        client->send(ConversionUtils::str2bytes(request));
      }
    });

    client->setOnMessage([](const std::vector<uint8_t> &data) {
      std::string msg(data.begin(), data.end());
      std::cout << "Client received: " << msg << "\n";
    });
    client->connect(address, std::to_string(port));
    client->start();
  });

  return 0;
}
