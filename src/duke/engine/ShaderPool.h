/*
 * ShaderPool.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef SHADERPOOL_H_
#define SHADERPOOL_H_

#include <duke/engine/ShaderFactory.h>
#include <map>

namespace duke {

struct ShaderPool {
	SharedProgram get(const ShaderDescription& key) {
		auto pFound = m_Map.find(key);
		if (pFound == m_Map.end())
			pFound = m_Map.insert(std::make_pair(key, buildProgram(key))).first;
		return pFound->second;
	}
private:
	std::map<ShaderDescription, SharedProgram> m_Map;
};

}  // namespace duke

#endif /* SHADERPOOL_H_ */
