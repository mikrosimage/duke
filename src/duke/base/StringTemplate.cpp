#include "duke/base/StringTemplate.hpp"

#include "duke/base/Check.hpp"
#include "duke/base/StringUtils.hpp"

#include <algorithm>

StringTemplate::Chunk::Chunk(StringSlice slice, Chunk::Type type) : slice(slice), type(type) {}

StringTemplate::StringTemplate(const char* templateString) : m_Template(templateString) {
  StringSlice remainder(m_Template);
  const auto pieces = split(remainder, '#');
  CHECK(std::count(remainder.begin(), remainder.end(), '#') % 2 == 0) << "Imbalanced # pairs in template string "
                                                                      << templateString;
  bool isString = true;
  for (StringSlice piece : pieces) {
    m_Chunks.emplace_back(piece, isString ? Chunk::STRING : Chunk::PARAMETER);
    isString = !isString;
  }
}

void StringTemplate::instantiate(const std::vector<std::pair<StringSlice, StringSlice>>& parameters,
                                 StringAppender& output) const {
  for (const auto& chunk : m_Chunks) {
    switch (chunk.type) {
      case Chunk::STRING:
        output.append(chunk.slice);
        break;
      case Chunk::PARAMETER:
        const auto pFound = std::find_if(
            parameters.begin(), parameters.end(),
            [&chunk](const std::pair<StringSlice, StringSlice>& pair) { return pair.first == chunk.slice; });
        CHECK(pFound != parameters.end()) << "no value for parameter '" << chunk.slice << "'";
        output.append(pFound->second);
        break;
    }
  }
}
