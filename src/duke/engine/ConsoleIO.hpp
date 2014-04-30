#pragma once

#include <concurrent/queue.hpp>
#include <thread>
#include <string>
#include <vector>

struct ConsoleIO {
  ConsoleIO();
  ~ConsoleIO();
  void poll(std::vector<std::string>& strings);

 private:
  void run();

  concurrent::queue<std::string> commands;
  std::thread m_Poller;
};
