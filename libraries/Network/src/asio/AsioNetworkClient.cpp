#include "network/implementations/asio/AsioNetworkClient.hpp"
#include <arpa/inet.h> // Для ntohl и htonl
#include <iostream>

namespace network {

AsioNetworkClient::AsioNetworkClient() : resolver_(io_), socket_(io_) {}

void AsioNetworkClient::start() { io_.run(); }

void AsioNetworkClient::stop() {
  disconnect();
  io_.stop();
}

void AsioNetworkClient::connect(const std::string &host,
                                const std::string &port) {
  if (!is_active_) {
    std::cerr << "Client is not active. Cannot connect.\n";
    return;
  }
  boost::asio::post(socket_.get_executor(), [this, host, port,
                                             self = shared_from_this()]() {
    if (!is_active_)
      return;
    resolver_.async_resolve(
        host, port,
        [this, self](const boost::system::error_code &resolve_ec,
                     boost::asio::ip::tcp::resolver::results_type endpoints) {
          if (!self->is_active_)
            return;
          if (resolve_ec) {
            std::cerr << "Resolve error: " << resolve_ec.message() << "\n";
            if (on_disconnect_handler_) {
              on_disconnect_handler_();
            }
            return;
          }
          boost::asio::async_connect(
              self->socket_, endpoints,
              [this, self](const boost::system::error_code &connect_ec,
                           const boost::asio::ip::tcp::endpoint &) {
                if (!self->is_active_)
                  return;
                if (!connect_ec) {
                  if (on_connect_handler_) {
                    on_connect_handler_();
                  }
                  startReading(); // начинаем чтение только после успешного
                                  // подключения
                } else {
                  std::cerr << "Connect error: " << connect_ec.message()
                            << "\n";
                  if (on_disconnect_handler_) {
                    on_disconnect_handler_();
                  }
                }
              });
        });
  });
}

void AsioNetworkClient::send(const std::vector<uint8_t> &data) {
  uint32_t size = data.size();
  uint32_t network_size =
      htonl(size); // Преобразуем размер в сетевой порядок байтов
  std::vector<uint8_t> buffer;
  buffer.reserve(sizeof(network_size) + data.size());

  // Добавляем размер в начало сообщения
  buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&network_size),
                reinterpret_cast<uint8_t *>(&network_size) +
                    sizeof(network_size));
  // Добавляем само сообщение
  buffer.insert(buffer.end(), data.begin(), data.end());

  boost::asio::post(io_, [this, buffer = std::move(buffer)]() mutable {
    if (!is_active_ || !socket_.is_open())
      return;
    bool should_start = false;
    {
      std::lock_guard<std::mutex> lock(write_queue_mutex_);
      should_start = write_queue_.empty() && !is_writing_;
      write_queue_.push_back(buffer);
    }
    if (should_start) {
      doWrite();
    }
  });
}

void AsioNetworkClient::doWrite() {
  std::vector<uint8_t> current_data;
  {
    std::lock_guard<std::mutex> lock(write_queue_mutex_);
    if (write_queue_.empty()) {
      is_writing_ = false;
      return;
    }
    current_data = write_queue_.front();
  }
  boost::asio::async_write(
      socket_, boost::asio::buffer(current_data),
      [this, self = shared_from_this()](const boost::system::error_code &ec,
                                        size_t /*length*/) {
        if (!self->is_active_)
          return;
        if (!ec) {
          {
            std::lock_guard<std::mutex> lock(self->write_queue_mutex_);
            self->write_queue_.pop_front();
            self->is_writing_ = false;
          }
          self->doWrite(); // продолжаем отправку
        } else {
          std::cerr << "Write error: " << ec.message() << std::endl;
          self->socket_.close();
        }
      });
}

void AsioNetworkClient::setOnConnect(std::function<void()> handler) {
  on_connect_handler_ = handler;
}

void AsioNetworkClient::setOnDisconnect(std::function<void()> handler) {
  on_disconnect_handler_ = handler;
}

void AsioNetworkClient::setOnMessage(
    std::function<void(const std::vector<uint8_t> &)> handler) {
  on_message_handler_ = handler;
}

void AsioNetworkClient::poll() { io_.poll(); }

void AsioNetworkClient::setSignals(std::shared_ptr<Signal> signals) {
  signals_ = signals;
}

bool AsioNetworkClient::isConnected() { return socket_.is_open(); }

void AsioNetworkClient::startReading() {
  if (!is_active_)
    return;

  // Сначала читаем размер сообщения (4 байта)
  boost::asio::async_read(
      socket_, boost::asio::buffer(&message_size_, sizeof(message_size_)),
      [this, self = shared_from_this()](const boost::system::error_code &ec,
                                        size_t /*length*/) {
        if (ec || !self->is_active_) {
          return handleReadError(ec);
        }

        // Преобразуем размер из сетевого формата в локальный
        message_size_ = ntohl(message_size_);

        // Теперь читаем само сообщение согласно полученному размеру
        message_buffer_.resize(message_size_);
        boost::asio::async_read(
            socket_, boost::asio::buffer(message_buffer_),
            [this, self](const boost::system::error_code &ec,
                         size_t /*length*/) {
              if (ec || !self->is_active_) {
                return handleReadError(ec);
              }

              if (on_message_handler_) {
                on_message_handler_(message_buffer_);
              }
              startReading(); // продолжаем чтение
            });
      });
}

void AsioNetworkClient::handleReadError(const boost::system::error_code &ec) {
  if (ec == boost::asio::error::eof || !socket_.is_open()) {
    std::cerr << "Disconnected gracefully\n";
  } else if (ec) {
    std::cerr << "Read error: " << ec.message() << "\n";
  }
  is_active_ = false;
  if (on_disconnect_handler_) {
    on_disconnect_handler_();
  }
}

void AsioNetworkClient::disconnect() {
  boost::asio::post(io_, [this]() {
    if (socket_.is_open()) {
      socket_.close();
    }
  });
}

} // namespace network
