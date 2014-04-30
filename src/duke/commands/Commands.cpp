#include "Commands.hpp"
#include <memory>

using namespace std;

namespace cmd {

Command* Commands::create(std::string token) const {
  const auto pCmd = find(token);
  if (!pCmd) return nullptr;
  return pCmd->create();
}

std::string Commands::execute(const std::string& cmd) const {
  string token;
  istringstream iss(cmd);
  if (!(iss >> token)) return "";
  unique_ptr<Command> pCmd(create(token));
  if (!pCmd) return "unknown '" + token + "' command";
  const auto error = pCmd->parseArguments(iss);
  if (!error.empty()) return error;
  return pCmd->execute();
}

}  // namespace cmd
