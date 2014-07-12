#include "duke/shading/ShadingModel.hpp"

#include "duke/base/Check.hpp"
#include "duke/base/StringUtils.hpp"

#include <algorithm>
#include <cctype>

namespace duke {

namespace shader {

namespace {

bool is_space(char c) { return isspace(c); }
bool is_opening_paren(char c) { return c == '('; }
bool is_closing_paren(char c) { return c == ')'; }
bool is_opening_brace(char c) { return c == '{'; }

}  // namespace

Function::Function(const std::string& function, const std::vector<std::string>& parameters_)
    : function(function), parameters(parameters_) {
  for (const auto& parameter : parameters) {
    const auto index = parameter.rfind(' ');
    CHECK(index != std::string::npos);
    StringSlice type(parameter.data(), index);
    StringSlice name(parameter.data() + index, parameter.size() - index);
    params.emplace_back(trim(type), trim(name));
  }
  StringSlice input = function;
  CHECK(!input.empty() && std::isalpha(input.front()) && input.back() == '}');
  returntype = consumeUntil(input, &is_space);
  CHECK(!returntype.empty());
  name = trim(consumeUntil(input, &is_opening_paren));
  input = pop_front(input);  // '('
  for (auto variable : split(consumeUntil(input, &is_closing_paren), ',')) {
    variable = trim(variable);
    const auto pieces = split(variable, ' ');
    CHECK(pieces.size() == 2);
    args.emplace_back(pieces.at(0), pieces.at(1));
  }
  input = pop_front(input);  // ')'
  consumeUntil(input, &is_opening_brace);
}

}  // namespace shader

}  // namespace duke
