#include <boost/stacktrace/stacktrace.hpp>
#include <crashHandler/implementation/boostStackTrace.hpp>
#include <csignal>
#include <functional>
#include <iostream>

void signal_handler(int signal) {
  std::cerr << "Signal caught (" << signal << "): dumping stack trace\n";
  std::cerr << boost::stacktrace::stacktrace() << std::endl;
  std::_Exit(signal); // или exit(signal)
}

void BoostStackTrace::init(std::function<void()> func) {
  std::signal(SIGABRT, signal_handler);
  std::signal(SIGSEGV, signal_handler);
  std::signal(SIGILL, signal_handler);

  try {
    func();
  }

  catch (const std::exception &e) {
    // Выводим сообщение об ошибке
    std::cerr << "Exception caught: " << e.what() << std::endl;

    // Выводим трассировку стека
    std::cerr << "Stack trace:\n"
              << boost::stacktrace::stacktrace() << std::endl;
  }
}
