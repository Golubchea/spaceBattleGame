#include "network/NetworkFactory.hpp"
#include "network/implementations/asio/AsioNetworkClient.hpp"
#include "network/implementations/asio/AsioNetworkServer.hpp"

namespace network {

std::shared_ptr<INetworkClient> NetworkFactory::createClient() {
  return std::make_shared<AsioNetworkClient>();
}

std::shared_ptr<INetworkServer>
NetworkFactory::createServer(unsigned short port) {
  return std::make_shared<AsioNetworkServer>(port);
}

} // namespace network
