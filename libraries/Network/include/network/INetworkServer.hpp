#pragma once
#include <network/ISession.hpp>

namespace network {
class INetworkServer {
public:
  using Ptr = std::shared_ptr<INetworkServer>;

  std::vector<ISession::Ptr> clients_;

  virtual ~INetworkServer() = default;

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void poll() = 0;
  virtual void broadcast(const std::vector<uint8_t> &data) = 0;
  virtual void
  setOnClientConnect(std::function<void(ISession::Ptr session)> handler) = 0;
  virtual void eraseClient(ISession::Ptr session) = 0;
  inline std::vector<ISession::Ptr> clients() const { return clients_; }
};
} // namespace network
