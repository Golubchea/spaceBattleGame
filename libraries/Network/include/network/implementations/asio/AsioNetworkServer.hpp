#include <boost/asio.hpp>
#include <network/INetworkServer.hpp>
#include <network/implementations/asio/AsioSession.hpp>
#include <vector>

namespace network {

class AsioNetworkServer : public INetworkServer {
public:
  explicit AsioNetworkServer(unsigned short port);
  void start() override;
  void stop() override;
  void poll() override;
  void broadcast(const std::vector<uint8_t> &data) override;
  void setOnClientConnect(std::function<void(ISession::Ptr session)>) override;
  void eraseClient(ISession::Ptr session) override;

private:
  void doAccept();

  boost::asio::io_context io_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::vector<ISession::Ptr> clients_;
  std::function<void(ISession::Ptr session)> on_client_connect_;
};

} // namespace network
