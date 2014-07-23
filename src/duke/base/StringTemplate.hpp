#pragma once

#include "duke/base/StringAppender.hpp"
#include "duke/base/StringSlice.hpp"

#include <string>
#include <utility>
#include <vector>

struct StringTemplate {
  StringTemplate(const char* templateString);

  void instantiate(const std::vector<std::pair<StringSlice, StringSlice>>& parameters, StringAppender& output) const;

 private:
  struct Chunk {
    StringSlice slice;
    enum Type : bool {
      PARAMETER,
      STRING
    } type;
    Chunk(StringSlice slice, Type type);
  };

  const std::string m_Template;
  std::vector<Chunk> m_Chunks;
};
