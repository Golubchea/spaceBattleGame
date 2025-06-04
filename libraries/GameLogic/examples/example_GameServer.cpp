#include "Visualization/Visualization.hpp"
#include <ECSEngineLib/World.hpp>
#include <InversionOfControl/InversionOfControl.h>
#include <ServerLogic/Player.hpp>
#include <ServerLogic/PlayerFactory.hpp>
#include <iostream>
#include <network/INetworkServer.hpp>
#include <network/NetworkFactory.hpp>

#include <CommandLib/Commands/MacroCommand.hpp>
#include <CommandLib/Commands/MoveCommand.hpp>
#include <CommandLib/Commands/MoveWithFuelCommand.hpp>
#include <CommandLib/Commands/RotateCommand.hpp>
#include <ServerLogic/CommandExecutor.hpp>
#include <ServerLogic/CommandService.hpp>
#include <crashHandler/stacktrace.hpp>

int main(int, const char **) {

  // auto crashHandler = StackTraceFactory::createStackTrace();

  // crashHandler->init([]() {
  auto &container = IoCContainer::GetInstance();

  std::shared_ptr<World> world = std::make_shared<World>();
  container.RegisterFactory<World>("World", [&world]() { return world; });

  // Регистрация PlayerFactory
  container.RegisterFactory<PlayerFactory>("PlayerFactory", [&container]() {
    auto world = container.Resolve<std::shared_ptr<World>>("World");
    return std::make_shared<DefaultPlayerFactory>(world);
  });
  // Регистрация CommandService
  auto commandExecutor = std::make_shared<CommandExecutor>([world]() {
    std::cout << "Game state updated" << std::endl;
    std::cout << *world.get() << std::endl;
  });

  container.RegisterFactory<CommandExecutor>(
      "CommandService", [&commandExecutor]() { return commandExecutor; });

  std::shared_ptr<Signal> signals = std::make_shared<Signal>();

  Visualization visualization(signals, world, 800, 600, "Server");

  // Сервер
  auto networkServer = network::NetworkFactory::createServer(8080);

  networkServer->setOnClientConnect([&container](ISession::Ptr session) {
    auto worldService = container.Resolve<std::shared_ptr<World>>("World");
    auto playerFactory =
        container.Resolve<std::shared_ptr<PlayerFactory>>("PlayerFactory");
    auto commandService =
        container.Resolve<std::shared_ptr<CommandExecutor>>("CommandService");

    auto player = playerFactory->CreatePlayer({0, 0, 0}, {0, 0, 0}, 0, 100, 5);
    auto axis = playerFactory->CreateAxis();
    auto grid = playerFactory->CreateGrid();

    std::cout << "Player connected with Entity ID: " << player->entity() << "\n"
              << std::flush;

    session->setOnMessage(
        [commandService, player](const std::vector<uint8_t> &data) {
          std::string msg(data.begin(), data.end());
          auto deserialized =
              ICommand::Deserialize(data.data(), data.size(), nullptr);
          std::cout << "Received from player " << player->entity() << ": "
                    << deserialized->Type() << "\n"
                    << std::flush;
          commandService->EnqueueCommand(deserialized, player);
          commandService->WorkerLoop();
        });

    session->setOnDisconnect([player, worldService]() {
      worldService->DestroyEntity(player->entity());
      std::cout << "Player disconnected: Entity ID " << player->entity() << "\n"
                << *worldService << std::flush;
    });
  });

  while (!visualization.isClosed()) {
    networkServer->poll(); // Обработка событий
    visualization.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  networkServer->start();
  //});
  return 0;
}
