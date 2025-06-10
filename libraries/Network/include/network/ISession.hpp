#pragma once
#include <functional>
#include <memory>
#include <vector>

class ISession {
public:
    using Ptr = std::shared_ptr<ISession>;

    virtual ~ISession() = default;

    virtual void send(const std::vector<uint8_t>& data) = 0;
    virtual void close() = 0;

    virtual void setOnMessage(std::function<void(const std::vector<uint8_t>&)> handler) = 0;
    virtual void setOnConnect(std::function<void()> handler) = 0;
    virtual void setOnDisconnect(std::function<void()> handler) = 0;

    // ===== Новые методы для аутентификации ====
    virtual bool isAuthenticated() const = 0;
    virtual void setAuthenticated(bool value) = 0;
    virtual std::string getUsername() const = 0;
    virtual void setUsername(const std::string &username) = 0;
};
