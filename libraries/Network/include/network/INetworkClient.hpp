#pragma once
#include <Signals/Signals.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class INetworkClient {
public:
  using Ptr = std::shared_ptr<INetworkClient>;
  virtual ~INetworkClient() = default;

  virtual void connect(const std::string &host, const std::string &port) = 0;
  virtual void disconnect() = 0;
  virtual void send(const std::vector<uint8_t> &data) = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void setOnConnect(std::function<void()> handler) = 0;
  virtual void setOnDisconnect(std::function<void()> handler) = 0;
  virtual void
  setOnMessage(std::function<void(const std::vector<uint8_t> &)> handler) = 0;
  virtual void poll() = 0;
  virtual void setSignals(std::shared_ptr<Signal> signals) = 0;
  virtual bool isConnected() = 0;
};
