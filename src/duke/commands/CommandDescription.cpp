#include "CommandDescription.hpp"
#include <stdexcept>
#include <algorithm>

namespace cmd {

Command* CommandDescription::create() const {
	Command* pCmd = newInstanceFunc();
	pCmd->m_pDescription = this;
	return pCmd;
}

Command::~Command() {
}

std::string Command::parseArguments(std::istream &stream) {
	auto result = doParseArguments(stream);
	if (!result.empty())
		return result;
	getline(stream, result);
	result.erase(std::remove_if(begin(result), end(result), isspace), end(result));
	if (!result.empty())
		return "too many arguments";
	return {};
}

std::string Command::doParseArguments(std::istream &stream) {
	return {};
}

std::ostream& Command::serialize(std::ostream& stream) const {
	stream << m_pDescription->name;
	return doSerialize(stream);
}

std::ostream& Command::doSerialize(std::ostream& stream) const {
	return stream;
}

const char* getString(CommandPlaceHolder placeHolder) {
	switch (placeHolder) {
	case cmd:
		return "cmd";
	case path:
		return "path";
	case param:
		return "param";
	case value:
		return "value";
	}
	throw std::runtime_error("Unexpected enum");
}

}  // namespace cmd
