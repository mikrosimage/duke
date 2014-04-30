#include "ConsoleIO.hpp"
#include <iostream>

using namespace std;

ConsoleIO::ConsoleIO() {
  m_Poller = thread(&ConsoleIO::run, this);
  m_Poller.detach();
}

ConsoleIO::~ConsoleIO() {}

void ConsoleIO::poll(std::vector<std::string>& strings) { commands.drainTo(strings); }

void ConsoleIO::run() {
  std::string line;
  while (getline(cin, line)) commands.push(line);
}
