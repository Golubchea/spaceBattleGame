#pragma once
#include <functional>
#include <vector>
#include <mutex>

class Signal {
public:
    using Callback = std::function<void(int)>;
    
    void connect(Callback cb) {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_.push_back(cb);
    }
    
    void emit(int key) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& cb : callbacks_) {
            cb(key);
        }
    }

private:
    std::vector<Callback> callbacks_;
    std::mutex mutex_;
};
