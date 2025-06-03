
#include <crashHandler/implementation/boostStackTrace.hpp>
#include <crashHandler/stacktrace.hpp>

std::shared_ptr<IStackTrace> StackTraceFactory::createStackTrace() {
  return std::make_shared<BoostStackTrace>();
}
