#include "CommandLib/Commands/BurnFuelCommand.hpp"
#include "jwt-cpp/jwt.h"
#include <CommandLib/Commands/MoveCommand.hpp>
#include <CommandLib/Commands/RotateCommand.hpp>
#include <ECSEngineLib/World.hpp>
#include <InversionOfControl/InversionOfControl.h>
#include <ServerLogic/CommandExecutor.hpp>
#include <ServerLogic/CommandService.hpp>
#include <ServerLogic/Player.hpp>
#include <ServerLogic/PlayerFactory.hpp>
#include <Visualization/Visualization.hpp>
#include <crashHandler/stacktrace.hpp>
#include <iostream>
#include <network/INetworkServer.hpp>
#include <network/NetworkFactory.hpp>
#include <string>
#include <utils/conversionUtils.hpp>

std::string username = "admin";
std::string password = "password";

void send_auth_request(std::shared_ptr<INetworkClient> auth_session) {
  std::string payload = "user=" + username + ";pass=" + password;

  // generate_jwt(username);

  auth_session->send(ConversionUtils::str2bytes(payload));
}

int main(int, const char **) {

  auto auth_client = network::NetworkFactory::createClient();

  auth_client->setOnConnect([auth_client]() {
    std::cout << "[Client] Connected to auth server\n" << std::flush;
    send_auth_request(auth_client);
  });

  std::vector<uint8_t> receivedToken;
  bool tokenReceived = false;

  auth_client->setOnMessage(
      [&receivedToken, &tokenReceived](const std::vector<uint8_t> &data) {
        if (data.empty())
          return;

        if (data[0] == 0) {
          std::cerr << "[Client] Auth failed\n";
          return;
        }

        receivedToken = data;
        tokenReceived = true;
        std::cout << "[Client] Token received\n";
      });

  auth_client->connect("127.0.0.1", "8081"); // auth-server
  while (!tokenReceived) {
    auth_client->poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::shared_ptr<Signal> signals = std::make_shared<Signal>();

  auto &container = IoCContainer::GetInstance();
  std::shared_ptr<World> world = std::make_shared<World>();
  container.RegisterFactory<World>("World", [&world]() { return world; });

  // Регистрация PlayerFactory
  container.RegisterFactory<PlayerFactory>("PlayerFactory", [&container]() {
    auto world = container.Resolve<std::shared_ptr<World>>("World");
    return std::make_shared<DefaultPlayerFactory>(world);
  });
  // commandExecutor
  auto commandExecutor = std::make_shared<CommandExecutor>([world]() {
    std::cout << "Game state updated" << std::endl;
    //  std::cout << *world.get() << std::endl;
  });

  container.RegisterFactory<CommandExecutor>(
      "CommandService", [&commandExecutor]() { return commandExecutor; });

  auto world1 = container.Resolve<std::shared_ptr<World>>("World");
  Visualization visualization(signals, world1, 800, 600, "Client");

  auto factory =
      container.Resolve<std::shared_ptr<PlayerFactory>>("PlayerFactory");

  auto player = factory->CreatePlayer({0, 0, 0}, {0, 0, 0}, 0, 100, 5);
  auto axis = factory->CreateAxis();
  auto grid = factory->CreateGrid();

  // Подключение клиента

  std::string address = "127.0.0.1";
  std::string port = "8080";

  auto client = network::NetworkFactory::createClient();
  client->setSignals(signals);

  client->setOnConnect([&]() {
    // send_auth_request(client);

    std::cout << "Connected to server\n" << std::flush;

    signals->connect("up", [client, &commandExecutor, &player]() {
      std::array<double, 2> delta = {0.0f, 1.0f};
      auto command = std::make_shared<MoveCommand>(delta);
      commandExecutor->EnqueueCommand(command, player);
      commandExecutor->WorkerLoop();
      auto data = command->Serialize();
      client->send(data);
    });

    signals->connect("down", [client, &commandExecutor, &player]() {
      std::array<double, 2> delta{0, -1};
      auto moveCmd = std::make_shared<MoveCommand>(delta);
      commandExecutor->EnqueueCommand(moveCmd, player);
      commandExecutor->WorkerLoop();
      std::vector<uint8_t> data = moveCmd->Serialize();
      client->send(data);
    });

    signals->connect("right", [client, &commandExecutor, &player]() {
      float angle = 0.1f;
      auto moveCmd = std::make_shared<RotateCommand>(angle);
      commandExecutor->EnqueueCommand(moveCmd, player);
      commandExecutor->WorkerLoop();
      std::vector<uint8_t> data = moveCmd->Serialize();
      client->send(data);
    });

    signals->connect("left", [client, &commandExecutor, &player]() {
      float angle = -0.1;
      auto moveCmd = std::make_shared<RotateCommand>(angle);
      commandExecutor->EnqueueCommand(moveCmd, player);
      commandExecutor->WorkerLoop();
      std::vector<uint8_t> data = moveCmd->Serialize();
      client->send(data);
    });

    signals->connect("space", [client, &commandExecutor, &player]() {
      std::array<double, 2> delta{-1, 0};
      auto moveCmd = std::make_shared<MoveCommand>(delta);
      commandExecutor->EnqueueCommand(moveCmd, player);
      commandExecutor->WorkerLoop();
      std::vector<uint8_t> data = moveCmd->Serialize();
      client->send(data);
    });
  });

  client->setOnMessage([](const std::vector<uint8_t> &data) {
    std::string msg(data.begin(), data.end());
    std::cout << "Client received: " << msg << "\n" << std::flush;
  });

  client->connect(address, port);

  // Основной цикл клиента
  while (!client->isConnected()) {
    client->poll(); // Обработка событий
    visualization.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  while (!visualization.isClosed()) {
    client->poll(); // Обработка событий
    visualization.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
