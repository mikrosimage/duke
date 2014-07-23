#pragma once

#include "duke/base/StringAppender.hpp"
#include "duke/base/StringSlice.hpp"

#include <string>
#include <vector>

namespace duke {

namespace shader {

struct Variable : noncopyable {
  StringSlice fulltype;
  StringSlice name;
  Variable(StringSlice type, StringSlice name) : fulltype(type), name(name) {}
};

struct Function : noncopyable {
  Function(const StringSlice function, const std::vector<std::string>& parameters = {});

  const std::string code;
  const std::vector<std::string> parameters;
  const size_t hash;

  StringSlice returntype;
  StringSlice name;
  std::vector<Variable> args;
  std::vector<Variable> params;
};

}  // namespace shader

}  // namespace duke
