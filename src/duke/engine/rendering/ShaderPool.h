/*
 * ShaderPool.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/engine/rendering/ShaderFactory.h>
#include <map>

namespace duke {

struct ShaderPool {
	SharedProgram get(const ShaderDescription& key) const {
		auto pFound = m_Map.find(key);
		if (pFound == m_Map.end())
			pFound = m_Map.insert(std::make_pair(key, buildProgram(key))).first;
		return pFound->second;
	}
private:
	mutable std::map<ShaderDescription, SharedProgram> m_Map;
};

}  // namespace duke
