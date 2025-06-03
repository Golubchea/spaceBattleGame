#include "network/implementations/asio/AsioNetworkServer.hpp"
#include "network/implementations/asio/AsioSession.hpp"
#include <iostream>

namespace network {

AsioNetworkServer::AsioNetworkServer(unsigned short port)
    : acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                    port)) {
  doAccept();
}

void AsioNetworkServer::start() { io_.run(); }

void AsioNetworkServer::stop() { io_.stop(); }

void AsioNetworkServer::poll() { io_.poll(); }

void AsioNetworkServer::broadcast(const std::vector<uint8_t> &data) {
  for (auto &client : clients_) {
    client->send(data);
  }
}

void AsioNetworkServer::setOnClientConnect(
    std::function<void(ISession::Ptr session)> handler) {
  on_client_connect_ = handler;
}

void AsioNetworkServer::doAccept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      auto session = std::make_shared<AsioSession>(std::move(socket));
      session->setOnDisconnect([this, session]() {
        session->close();
        std::erase_if(clients_, [&](const ISession::Ptr &client) {
          return client.get() == session.get();
        });
      });
      if (on_client_connect_) {
        on_client_connect_(session);
      }
      clients_.push_back(session);
      session->startReading();
    }
    doAccept();
  });
}

} // namespace network
