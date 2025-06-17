#pragma once
#include "network/ISession.hpp"
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <deque>
#include <iostream>
#include <memory>

namespace network {

class AsioSession : public ISession,
                    public std::enable_shared_from_this<AsioSession> {
public:
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf read_buffer_;
  std::function<void(const std::vector<uint8_t> &)> on_message_handler_;
  std::function<void()> on_connect_handler_;
  std::function<void()> on_disconnect_handler_;
  std::deque<std::vector<uint8_t>> write_queue_;
  bool is_writing_ = false;
  bool is_active_ = true;

  explicit AsioSession(boost::asio::ip::tcp::socket socket)
      : socket_(std::move(socket)) {}

  void send(const std::vector<uint8_t> &data) override {
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

    boost::asio::post(
        socket_.get_executor(), [this, buffer = std::move(buffer)]() mutable {
          if (!is_active_ || !socket_.is_open())
            return;
          bool should_start = write_queue_.empty() && !is_writing_;
          write_queue_.push_back(std::move(buffer));
          if (should_start) {
            doWrite();
          }
        });
  }

  void setOnMessage(
      std::function<void(const std::vector<uint8_t> &)> handler) override {
    on_message_handler_ = std::move(handler);
  }

  void setOnConnect(std::function<void()> handler) override {
    on_connect_handler_ = std::move(handler);
  }

  void setOnDisconnect(std::function<void()> handler) override {
    on_disconnect_handler_ = std::move(handler);
  }

  void startReading() {
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

  void handleReadError(const boost::system::error_code &ec) {
    if (ec == boost::asio::error::eof || !socket_.is_open()) {
      // Соединение закрыто нормально — штатное завершение
      std::cerr << "Client disconnected gracefully\n";
    } else if (ec) {
      std::cerr << "Read error: " << ec.message() << "\n";
    }
    is_active_ = false;
    if (on_disconnect_handler_) {
      on_disconnect_handler_();
    }
  }

  void doWrite() {
    if (!is_active_ || write_queue_.empty()) {
      is_writing_ = false;
      return;
    }
    is_writing_ = true;
    auto data = write_queue_.front();
    boost::asio::async_write(
        socket_, boost::asio::buffer(data),
        [this, self = shared_from_this()](const boost::system::error_code &ec,
                                          size_t /*length*/) {
          if (!self->is_active_)
            return;
          if (!ec) {
            self->write_queue_.pop_front();
            self->is_writing_ = false;
            self->doWrite();
          } else {
            std::cerr << "Write error: " << ec.message() << std::endl;
            self->close();
          }
        });
  }

  void close() override {
    boost::asio::post(socket_.get_executor(),
                      [this, self = shared_from_this()]() {
                        if (!is_active_)
                          return;
                        is_active_ = false;
                        if (on_disconnect_handler_) {
                          on_disconnect_handler_();
                        }
                        socket_.close();
                      });
  }

  // ==== Реализация новых методов ====
  bool isAuthenticated() const override { return authenticated_; }
  void setAuthenticated(bool value) override { authenticated_ = value; }

  std::string getUsername() const override { return username_; }
  void setUsername(const std::string &username) override {
    username_ = username;
  }

private:
  bool authenticated_ = false;
  std::string username_;
  uint32_t message_size_ = 0;
  std::vector<uint8_t> message_buffer_;
};

} // namespace network
