#pragma once

#include "duke/base/StringAppender.hpp"
#include "duke/base/StringSlice.hpp"

#include <string>
#include <vector>

namespace duke {

namespace shader {

struct Variable {
  StringSlice fulltype;
  StringSlice name;
  Variable(StringSlice type, StringSlice name) : fulltype(type), name(name) {}
};

struct Function {
  Function(const std::string& function, const std::vector<std::string>& parameters = {});

  std::string function;
  std::vector<std::string> parameters;

  StringSlice returntype;
  StringSlice name;
  std::vector<Variable> args;
  std::vector<Variable> params;
};

}  // namespace shader

}  // namespace duke
