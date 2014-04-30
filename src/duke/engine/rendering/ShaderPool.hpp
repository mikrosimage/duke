#pragma once

#include <duke/engine/rendering/ShaderFactory.hpp>
#include <map>

namespace duke {

struct ShaderPool {
  SharedProgram get(const ShaderDescription& key) const {
    auto pFound = m_Map.find(key);
    if (pFound == m_Map.end()) pFound = m_Map.insert(std::make_pair(key, buildProgram(key))).first;
    return pFound->second;
  }

 private:
  mutable std::map<ShaderDescription, SharedProgram> m_Map;
};

}  // namespace duke
