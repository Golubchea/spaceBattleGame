#include <iostream>
#include <jwt-cpp/jwt.h>
#include <network/NetworkFactory.hpp>
#include <thread>

// БД пользователей
std::unordered_map<std::string, std::string> users = {{"admin", "password"},
                                                      {"player1", "pass123"}};

// ==== Генерация JWT ====
std::vector<uint8_t> generate_jwt(const std::string &username) {
  auto token = jwt::create()
                   .set_type("JWT")
                   .set_issuer("game-server")
                   .set_subject(username)
                   .set_issued_at(std::chrono::system_clock::now())
                   .set_expires_at(std::chrono::system_clock::now() +
                                   std::chrono::minutes(5))
                   .sign(jwt::algorithm::hs256{"secret-key"});

  return std::vector<uint8_t>(token.begin(), token.end());
}

int main() {
  auto server = network::NetworkFactory::createServer(8081);

  server->setOnClientConnect([](ISession::Ptr session) {
    session->setOnMessage([session](const std::vector<uint8_t> &data) {
      std::string msg(data.begin(), data.end());

      // Пример формата сообщения: "user=admin;pass=password"
      std::string user_key = "user=";
      std::string pass_key = ";pass=";

      size_t user_pos = msg.find(user_key);
      size_t pass_pos = msg.find(pass_key);

      if (user_pos == std::string::npos || pass_pos == std::string::npos) {
        session->send({0}); // Ошибка формата
        return;
      }

      std::string username =
          msg.substr(user_key.size(), pass_pos - user_pos - user_key.size());
      std::string password = msg.substr(pass_pos + pass_key.size());

      // Проверяем логин/пароль
      if (users.find(username) != users.end() && users[username] == password) {
        std::cout << "[Auth] Login successful for " << username << "\n";
        session->send(generate_jwt(username));
      } else {
        std::cerr << "[Auth] Failed login for " << username << "\n";
        session->send({1});
      }
    });
  });

  std::cout << "[AuthServer] Running on port 8081\n";
  server->start();

  return 0;
}
