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

#include <ServerLogic/Systems/CollisionSystem.hpp>

#include <jwt-cpp/jwt.h>
#include <utils/conversionUtils.hpp>

bool verify_jwt_token(const std::vector<uint8_t> &data, std::string *username) {
  std::string token(data.begin(), data.end());

  try {
    auto decoded = jwt::decode(token);
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{"secret-key"})
                        .with_issuer("game-server")
                        .with_type("JWT");

    verifier.verify(decoded);

    if (username) {
      *username = decoded.get_subject(); // обычно это "sub"
    }

    return true;

  } catch (const std::exception &e) {
    std::cerr << "[JWT Error] " << e.what() << "\n";
    return false;
  }
}

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

  std::vector<EntityRect>
      initialObjects; // может быть пустым — будет заполнен позже
  auto bvh = std::make_shared<DynamicBVH>(initialObjects);
  container.RegisterFactory<DynamicBVH>("DynamicBVH", [bvh]() { return bvh; });

  // Регистрация CommandService
  auto commandExecutor = std::make_shared<CommandExecutor>([world]() {
    std::cout << "Game state updated" << std::endl;
    // std::cout << *world.get() << std::endl;
    world->Update(1);
  });

  container.RegisterFactory<CommandExecutor>(
      "CommandService", [&commandExecutor]() { return commandExecutor; });

  std::shared_ptr<Signal> signals = std::make_shared<Signal>();

  Visualization visualization(signals, world, 800, 600, "Server");

  auto executor =
      container.Resolve<std::shared_ptr<CommandExecutor>>("CommandService");

  auto system = std::make_shared<CollisionSystem>(world, bvh, executor);

  // Сервер
  auto networkServer = network::NetworkFactory::createServer(8080);

  networkServer->setOnClientConnect([&container,
                                     networkServer](ISession::Ptr session) {
    auto worldService = container.Resolve<std::shared_ptr<World>>("World");
    auto playerFactory =
        container.Resolve<std::shared_ptr<PlayerFactory>>("PlayerFactory");
    auto commandService =
        container.Resolve<std::shared_ptr<CommandExecutor>>("CommandService");

    auto player = playerFactory->CreatePlayer({0, 0, 0}, {0, 0, 0}, 0, 100, 5);
    auto axis = playerFactory->CreateAxis();
    auto grid = playerFactory->CreateGrid();

    session->setOnMessage([commandService, session, playerFactory,
                           player](const std::vector<uint8_t> &data) {
      if (!session->isAuthenticated()) {
        std::string username;
        if (!verify_jwt_token(data, &username)) {
          std::cerr << "[GameServer] Invalid token from client\n";
          // session->send({1}); // Ошибка авторизации
          // session->close();
          // return;
        }

        session->setUsername(username);
        session->setAuthenticated(true);

        session->send({0}); // Успешная аутентификация
        std::cout << "[GameServer] Auth OK for: " << username << "\n";

        // Здесь можно создать игрока, если нужно

        std::cout << "Player connected with Entity ID: " << player->entity()
                  << "\n"
                  << std::flush;

      } else {
        std::string msg(data.begin(), data.end());
        auto deserialized =
            ICommand::Deserialize(data.data(), data.size(), nullptr);
        std::cout << "Received from player " << player->entity() << ": "
                  << deserialized->Type() << "\n"
                  << std::flush;
        commandService->EnqueueCommand(deserialized, player);
        commandService->WorkerLoop();
      }
    });

    // session->setOnMessage(
    //     [commandService, player, session](const std::vector<uint8_t> &data) {
    //       // Проверяем токен
    //       std::string username;
    //       if (!verify_jwt_token(data, &username)) {
    //         std::cerr << "[GameServer] Invalid token from client\n";
    //         session->send({1}); // ошибка авторизации
    //         session->close();
    //         return;
    //       }
    //       session->send({0}); // успешная аутентификация

    //       std::cout << "[GameServer] Auth OK for: " << username << "\n";

    //     });

    session->setOnDisconnect([player, worldService, &session,
                              &networkServer]() {
      worldService->DestroyEntity(player->entity());
      //  clients_.remove(session); // например, из списка активных клиентов
      std::cout << "Player disconnected: Entity ID " << player->entity() << "\n"
                << std::flush;
      // << *worldService << std::flush;
    });
  });

  while (!visualization.isClosed()) {

    networkServer->poll(); // Обработка событий
    visualization.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  networkServer->start();

  return 0;
}
