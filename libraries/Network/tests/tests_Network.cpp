#include <gtest/gtest.h>
#include <network/NetworkFactory.hpp>
#include <utils/conversionUtils.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <network/INetworkServer.hpp>
#include <network/ISession.hpp>
#include <thread>
#include <vector>

class MockNetworkServer : public network::INetworkServer {
public:
  void start() override {};
  void stop() override {};
  void poll() override {};
  void broadcast(const std::vector<uint8_t> &) override {};
  void setOnClientConnect(
      std::function<void(ISession::Ptr session)> handler) override {};
  void eraseClient(ISession::Ptr session) override {};
};

class MockSession : public ISession {
  void send(const std::vector<uint8_t> &data) override {};
  void close() override {};
  void setOnMessage(
      std::function<void(const std::vector<uint8_t> &)> handler) override {};
  void setOnConnect(std::function<void()> handler) override {};
  void setOnDisconnect(std::function<void()> handler) override {};

  bool isAuthenticated() const override { return authenticated_; }
  void setAuthenticated(bool value) override { authenticated_ = value; }

  std::string getUsername() const override { return username_; }
  void setUsername(const std::string &username) override {
    username_ = username;
  }

private:
  bool authenticated_ = false;
  std::string username_;
};

// === Тест: клиент и сервер обмениваются сообщениями ===
TEST(NetworkLibTest, ClientCanConnectToServer) {

  const int port = 9877;

  auto server = network::NetworkFactory::createServer(port);

  bool clientConnected = false;
  server->setOnClientConnect([&](ISession::Ptr session) {
    clientConnected = true;
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1)); // даем время обработать
    session->close();
  });

  std::thread serverThread([&server]() { server->start(); });

  auto client = network::NetworkFactory::createClient();
  client->connect("127.0.0.1", std::to_string(port));
  client->start();

  EXPECT_TRUE(clientConnected);

  client->stop();
  server->stop();

  if (serverThread.joinable())
    serverThread.join();
}

TEST(NetworkLibTest, ClientReceivesMessageFromServer) {
  const int port = 9878;

  auto server = network::NetworkFactory::createServer(port);

  bool clientConnected = false;
  bool messageReceived = false;

  server->setOnClientConnect([&](ISession::Ptr session) {
    clientConnected = true;
    session->setOnMessage([&messageReceived](const std::vector<uint8_t> &data) {
      std::string msg = ConversionUtils::bytes2str(data);
      EXPECT_EQ(msg, "hello");
      messageReceived = true;
    });
    session->send(ConversionUtils::str2bytes("serverToClient"));
  });

  std::thread serverThread([&server]() { server->start(); });

  auto client = network::NetworkFactory::createClient();
  client->connect("127.0.0.1", std::to_string(port));

  client->setOnMessage([&](const std::vector<uint8_t> &data) {
    std::string msg = ConversionUtils::bytes2str(data);
    EXPECT_EQ(msg, "serverToClient");
    client->disconnect();
  });

  client->setOnConnect(
      [&]() { client->send(ConversionUtils::str2bytes("hello")); });

  client->start();

  std::this_thread::sleep_for(
      std::chrono::milliseconds(100)); // ждем выполнения

  EXPECT_TRUE(clientConnected);
  EXPECT_TRUE(messageReceived);

  server->stop();
  if (serverThread.joinable())
    serverThread.join();
}

TEST(INetworkServerTest, Clients_ReturnsCorrectList) {
  MockNetworkServer server;

  // Добавляем mock-сессии
  auto session1 = std::make_shared<MockSession>();
  auto session2 = std::make_shared<MockSession>();

  server.clients_.push_back(session1);
  server.clients_.push_back(session2);

  auto result = server.clients();

  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], session1);
  EXPECT_EQ(result[1], session2);
}

// ==== ТЕСТ 4: Сервер может отправить эхо-сообщение клиенту ====
// TEST(NetworkLibTest, ServerEchoesMessageToClient) {
//   const int port = 9879;

//   auto server = network::NetworkFactory::createServer(port);

//   bool clientConnected = false;
//   bool echoReceived = false;

//   server->setOnClientConnect([&](ISession::Ptr session) {
//     clientConnected = true;
//     session->setOnMessage([=](const std::vector<uint8_t> &data) {
//       session->send(data); // Эхо
//     });
//   });

//   std::thread serverThread([&server]() { server->start(); });

//   auto client = network::NetworkFactory::createClient();
//   client->connect("127.0.0.1", std::to_string(port));

//   client->setOnMessage([&](const std::vector<uint8_t> &data) {
//     std::string msg = ConversionUtils::bytes2str(data);
//     EXPECT_EQ(msg, "ping");
//     echoReceived = true;
//     client->disconnect();
//   });

//   client->setOnConnect(
//       [&]() { client->send(ConversionUtils::str2bytes("ping")); });

//   client->start();

//   std::this_thread::sleep_for(std::chrono::milliseconds(1500));

//   EXPECT_TRUE(clientConnected);
//   EXPECT_TRUE(echoReceived);

//   server->stop();
//   if (serverThread.joinable())
//     serverThread.join();
// }

// ==== ТЕСТ 5: Клиент корректно отключается ====
// TEST(NetworkLibTest, ClientDisconnectsGracefully) {
//   const int port = 9880;

//   auto server = network::NetworkFactory::createServer(port);

//   bool clientConnected = false;
//   bool clientDisconnected = false;

//   server->setOnClientConnect([&](ISession::Ptr session) {
//     clientConnected = true;
//     session->setOnDisconnect(
//         [&clientDisconnected]() { clientDisconnected = true; });
//   });

//   std::thread serverThread([&server]() { server->start(); });

//   auto client = network::NetworkFactory::createClient();
//   client->connect("127.0.0.1", std::to_string(port));
//   client->start();

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));
//   client->disconnect();

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));

//   EXPECT_TRUE(clientConnected);
//   EXPECT_TRUE(clientDisconnected);

//   server->stop();
//   if (serverThread.joinable())
//     serverThread.join();
// }

// ==== ТЕСТ 6: Сервер шлет сообщение всем клиентам ====
// TEST(NetworkLibTest, ServerBroadcastsToAllClients) {
//   const int port = 9881;

//   auto server = network::NetworkFactory::createServer(port);

//   std::atomic<int> clientsConnected{0};
//   std::atomic<int> messagesReceived{0};

//   server->setOnClientConnect([&](ISession::Ptr session) {
//     session->setOnMessage(
//         [&messagesReceived](const std::vector<uint8_t> &data) {
//           std::string msg = ConversionUtils::bytes2str(data);
//           EXPECT_EQ(msg, "broadcast_msg");
//           messagesReceived++;
//         });
//     clientsConnected++;
//   });

//   std::thread serverThread([&server]() { server->start(); });

//   auto client1 = network::NetworkFactory::createClient();
//   auto client2 = network::NetworkFactory::createClient();

//   client1->connect("127.0.0.1", std::to_string(port));
//   client2->connect("127.0.0.1", std::to_string(port));

//   client1->start();
//   client2->start();

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));

//   server->broadcast(ConversionUtils::str2bytes("broadcast_msg"));

//   std::this_thread::sleep_for(std::chrono::milliseconds(500));

//   EXPECT_EQ(clientsConnected.load(), 2);
//   EXPECT_EQ(messagesReceived.load(), 2);

//   client1->disconnect();
//   client2->disconnect();
//   server->stop();
//   if (serverThread.joinable())
//     serverThread.join();
// }
