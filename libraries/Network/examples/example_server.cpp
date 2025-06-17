
#include <crashHandler/stacktrace.hpp>
#include <network/NetworkFactory.hpp>
#include <utils/conversionUtils.hpp>

#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

namespace {
constexpr char responce[] = "serverToClient";
int port = 8080;
} // namespace

int main(int argc, char *argv[]) {
  auto crashHandler = StackTraceFactory::createStackTrace();

  crashHandler->init([]() {
    auto server = network::NetworkFactory::createServer(port);
    server->setOnClientConnect([](ISession::Ptr session) {
      std::cout << "New client connected\n";

      session->setOnMessage([=](const std::vector<uint8_t> &data) {
        std::cout << "Received from client " << ConversionUtils::bytes2str(data)
                  << "\n"
                  << std::flush;
        session->send(ConversionUtils::str2bytes(responce)); // эхо
      });
    });
    server->start();
  });

  return 0;
}
