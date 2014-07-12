#pragma once

#include "duke/base/StringAppender.hpp"
#include "duke/base/StringSlice.hpp"

#include <map>
#include <set>
#include <string>

struct StringTemplate {
  StringTemplate(const char* templateString);

  void instantiate(const std::map<StringSlice, StringSlice> parameters, StringAppender& output) const;

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
