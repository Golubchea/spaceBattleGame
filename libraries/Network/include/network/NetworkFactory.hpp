#pragma once
#include <network/INetworkClient.hpp>
#include <network/INetworkServer.hpp>

namespace network {

class NetworkFactory {
public:
  static std::shared_ptr<INetworkClient> createClient();
  static std::shared_ptr<INetworkServer> createServer(unsigned short port);
};

} // namespace network
