#include <Server/Entities/Commands.h>
#include <Server/Entities/ObjectManager.h>
#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>
using boost::asio::ip::tcp;

class GameServer
{
public:
    GameServer(boost::asio::io_context &io_context, int agent_port, int player_port)
        : agent_acceptor_(io_context, tcp::endpoint(tcp::v4(), agent_port))
        , player_acceptor_(io_context, tcp::endpoint(tcp::v4(), player_port))
        , next_agent_id_(1) // Счётчик для генерации ID
        , object_manager_() // Инициализация ObjectManager
    {
        start_agent_accept();
        start_player_accept();
    }

    void broadcast_to_players(const std::string &message)
    {
        for (auto &player : players_) {
            if (player->is_open()) {
                boost::asio::async_write(*player,
                                         boost::asio::buffer(message + "\n"),
                                         [this, player](const boost::system::error_code &error,
                                                        std::size_t /*length*/) {
                                             if (error) {
                                                 std::cerr << "Error broadcasting to player: "
                                                           << error.message() << std::endl;
                                                 remove_player(player);
                                             }
                                         });
            } else {
                remove_player(player);
            }
        }
    }

private:
    void start_agent_accept()
    {
        auto new_connection = std::make_shared<tcp::socket>(agent_acceptor_.get_executor());
        agent_acceptor_.async_accept(
            *new_connection, [this, new_connection](const boost::system::error_code &error) {
                if (!error) {
                    uint64_t agent_id = next_agent_id_++;
                    std::cout << "Agent connected: " << new_connection->remote_endpoint()
                              << " with ID: " << agent_id << std::endl;

                    agents_[agent_id] = new_connection;            // Сохраняем соединение с ID
                    command_manager_[agent_id] = CommandManager(); // Создаем CommandManager для агента

                    // Создаем объект для агента

                    const Vector<double, 2> &pos = {1, 1};
                    const Vector<double, 2> &vel = {1, 0};
                    auto object = std::make_shared<UObject>(pos, vel);
                    uint64_t object_id = object_manager_.createObject(object);
                    agent_objects_[agent_id] = object_id; // Привязываем объект к агенту
                    std::cout << "object created:" << *object;
                    handle_agent(new_connection, agent_id);
                } else {
                    std::cerr << "Error accepting agent: " << error.message() << std::endl;
                }
                start_agent_accept();
            });
    }

    void start_player_accept()
    {
        auto new_connection = std::make_shared<tcp::socket>(player_acceptor_.get_executor());
        player_acceptor_.async_accept(*new_connection,
                                      [this,
                                       new_connection](const boost::system::error_code &error) {
                                          if (!error) {
                                              std::cout << "Player connected: "
                                                        << new_connection->remote_endpoint()
                                                        << std::endl;
                                              players_.push_back(new_connection);
                                              handle_player(new_connection);
                                          } else {
                                              std::cerr
                                                  << "Error accepting player: " << error.message()
                                                  << std::endl;
                                          }
                                          start_player_accept();
                                      });
    }

    void handle_agent(std::shared_ptr<tcp::socket> socket, uint64_t agent_id)
    {
        auto buffer = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(
            *socket,
            *buffer,
            '\0', // Предполагается, что команды заканчиваются символом '\0'
            [this, socket, buffer, agent_id](const boost::system::error_code &error,
                                             std::size_t /*length*/) {
                if (!error && socket->is_open()) {
                    try {
                        std::istream is(buffer.get());
                        const std::vector<uint8_t> data((std::istreambuf_iterator<char>(is)),
                                                        std::istreambuf_iterator<char>());

                        uint64_t object_id = agent_objects_[agent_id];
                        auto object = object_manager_.getObjectById(object_id);

                        // Десериализуем команду
                        auto command = Command::Deserialize(data.data(), object);

                        // Добавляем команду в CommandManager
                        command_manager_[agent_id].AddCommand(command);
                        command_manager_[agent_id].ExecuteAll();

                        // Отправляем состояние всех объектов игрокам
                        broadcast_object_states();
                    } catch (const std::exception &e) {
                        std::cerr << "Error processing command for agent " << agent_id << ": "
                                  << e.what() << std::endl;
                    }

                    // Продолжаем чтение следующих данных
                    handle_agent(socket, agent_id);
                } else {
                    remove_agent(agent_id);
                }
            });
    }

    void broadcast_object_states()
    {
        // Формируем строку с состоянием всех объектов
        std::string message = "Object States:\n";
        for (const auto &[agent_id, object_id] : agent_objects_) {
            std::shared_ptr<UObject> object = object_manager_.getObjectById(object_id);

            message += "Agent ID: " + std::to_string(agent_id) + object->toString();
        }

        // Рассылаем сообщение всем игрокам
        broadcast_to_players(message);
    }

    void handle_player(std::shared_ptr<tcp::socket> socket)
    {
        auto buffer = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(
            *socket,
            *buffer,
            '\n',
            [this, socket, buffer](const boost::system::error_code &error, std::size_t /*length*/) {
                if (!error && socket->is_open()) {
                    std::istream is(buffer.get());
                    std::string message;
                    std::getline(is, message);
                    std::cout << "Received from player: " << message << std::endl;
                    // Отправляем подтверждение игроку
                    std::string response = "Command received: " + message;
                    boost::asio::async_write(*socket,
                                             boost::asio::buffer(response + "\n"),
                                             [this, socket](const boost::system::error_code &error,
                                                            std::size_t /*length*/) {
                                                 if (error) {
                                                     std::cerr
                                                         << "Error sending response to player: "
                                                         << error.message() << std::endl;
                                                     remove_player(socket);
                                                 }
                                             });
                    handle_player(socket);
                } else {
                    if (error) {
                        std::cerr << "Error reading from player: " << error.message() << std::endl;
                    }
                    remove_player(socket);
                }
            });
    }

    void remove_agent(uint64_t agent_id)
    {
        auto it = agents_.find(agent_id);
        if (it != agents_.end()) {
            std::cout << "Agent disconnected: ID " << agent_id << std::endl;

            // Удаляем объект, связанный с агентом
            uint64_t object_id = agent_objects_[agent_id];
            object_manager_.removeObject(object_id);

            agents_.erase(it);
            command_manager_.erase(agent_id); // Удаляем CommandManager для агента
            agent_objects_.erase(agent_id);   // Удаляем привязку объекта к агенту
        }
    }
    void remove_player(std::shared_ptr<tcp::socket> socket)
    {
        auto it = std::find(players_.begin(), players_.end(), socket);
        if (it != players_.end()) {
            std::cout << "Player disconnected: " << (*it)->remote_endpoint() << std::endl;
            players_.erase(it);
        }
    }
    std::vector<std::shared_ptr<tcp::socket>> players_;
    tcp::acceptor agent_acceptor_;
    tcp::acceptor player_acceptor_;
    std::unordered_map<uint64_t, std::shared_ptr<tcp::socket>> agents_; // Агенты с их ID
    std::unordered_map<uint64_t, CommandManager> command_manager_; // CommandManager для каждого агента
    std::unordered_map<uint64_t, uint64_t> agent_objects_;         // Привязка объектов к агентам
    ObjectManager object_manager_;                                 // Менеджер объектов
    uint64_t
        next_agent_id_; // Счётчик для генерации ID                                // Счётчик для генерации ID
};
