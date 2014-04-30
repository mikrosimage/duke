#pragma once

#include <duke/commands/Commands.hpp>
#include <duke/engine/parameters/Parameters.hpp>

#include <functional>

namespace cmd {

class CmdCmd : public Command {
 protected:
  const Commands &cmds;
  const CommandDescription *pCommandDescription;
  CmdCmd(const Commands &cmds);
};

class ManCmd : public CmdCmd {
 public:
  ManCmd(const Commands &cmds);
  virtual std::string execute();
  virtual std::string doParseArguments(std::istream &stream);
};

class ArgsCmd : public CmdCmd {
 public:
  ArgsCmd(const Commands &cmds);
  virtual std::string execute();
  virtual std::string doParseArguments(std::istream &stream);
};

class SuggestCmd : public CmdCmd {
  std::string value;

 public:
  SuggestCmd(const Commands &cmds);
  virtual std::string execute();
  virtual std::string doParseArguments(std::istream &stream);
};

class LsCmd : public Command {
  std::string path;

 public:
  virtual std::string execute();
  virtual std::string doParseArguments(std::istream &stream);
};

class NoOpCmd : public Command {
 public:
  virtual std::string execute();
};

class FunctionCmd : public Command {
  const std::function<void()> m_Function;

 public:
  FunctionCmd(const std::function<void()> &func);
  virtual std::string execute();
};

class SuggestParam : public Command {
  std::string value;
  const Parameters &params;

 public:
  SuggestParam(const Parameters &params);
  virtual std::string execute();
  virtual std::string doParseArguments(std::istream &stream);
};

}  // namespace cmd
