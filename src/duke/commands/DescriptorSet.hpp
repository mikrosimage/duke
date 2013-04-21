#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>

template<typename T>
struct DescriptorSet {
	T& add(T &&value) {
		return m_Map.insert(std::make_pair(value.name, std::move(value))).first->second;
	}
	const T* find(const std::string &keyword) const {
		auto pFound = m_Map.find(keyword);
		if (pFound == m_Map.end())
			return nullptr;
		return &pFound->second;
	}

	std::vector<std::string> suggest(const std::string &start) const {
		std::vector<std::string> suggestions;
		for (const auto& pair : m_Map)
			if (startsWith(pair.first, start))
				suggestions.emplace_back(pair.first);
		return suggestions;
	}
private:
	std::map<std::string, T> m_Map;

	static bool startsWith(const std::string &a, const std::string &b) {
		return a.compare(0, std::min(a.size(), b.size()), b) == 0;
	}
};

