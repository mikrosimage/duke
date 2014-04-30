#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <cassert>

namespace cmd {

enum CommandPlaceHolder {
  cmd,
  path,
  param,
  value
};

const char* getString(CommandPlaceHolder);

class Command;

struct CommandDescription {
  std::string name;
  std::string help;
  std::vector<CommandPlaceHolder> placeholders;
  std::function<Command*()> newInstanceFunc;

  Command* create() const;
};

class Command {
 public:
  virtual ~Command() = 0;
  virtual std::string execute() = 0;
  std::string parseArguments(std::istream& stream);
  std::ostream& serialize(std::ostream& stream) const;
  inline const CommandDescription& getDescription() const {
    assert(m_pDescription);
    return *m_pDescription;
  }

 protected:
  virtual std::string doParseArguments(std::istream& stream);
  virtual std::ostream& doSerialize(std::ostream& stream) const;
  friend struct CommandDescription;
  const CommandDescription* m_pDescription;
};

}  // namespace cmd
