#pragma once

#include <crashHandler/stacktrace.hpp>

class BoostStackTrace : public IStackTrace {
public:
  void init(std::function<void()> func) override;
};
