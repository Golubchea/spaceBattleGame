#include <boost/asio/ip/tcp.hpp>
#include <network/INetworkClient.hpp>
#include <network/implementations/asio/AsioSession.hpp>

namespace network {

class AsioNetworkClient
    : public INetworkClient,
      public std::enable_shared_from_this<AsioNetworkClient> {
public:
  AsioNetworkClient();
  void connect(const std::string &host, const std::string &port) override;
  void disconnect() override;
  void send(const std::vector<uint8_t> &data) override;

  void setOnConnect(std::function<void()> handler) override;
  void setOnDisconnect(std::function<void()> handler) override;
  void setOnMessage(
      std::function<void(const std::vector<uint8_t> &)> handler) override;

  void poll() override;
  void setSignals(std::shared_ptr<Signal> signals) override;

  bool isConnected() override;
  void start() override;
  void stop() override;

private:
  void handleReadError(const boost::system::error_code &ec);
  boost::asio::io_context io_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::ip::tcp::resolver resolver_;
  boost::asio::ip::tcp::resolver::results_type endpoints_;

  std::function<void()> on_connect_handler_;
  std::function<void()> on_disconnect_handler_;
  std::function<void(const std::vector<uint8_t> &)> on_message_handler_;

  std::shared_ptr<Signal> signals_ = nullptr;

  std::deque<std::vector<uint8_t>> write_queue_;
  std::mutex write_queue_mutex_;

  bool is_writing_ = false;
  bool is_active_ = true;

  boost::asio::streambuf read_buffer_;

  void doWrite();
  void startReading();

  uint32_t message_size_ = 0;
  std::vector<uint8_t> message_buffer_;
};

} // namespace network
