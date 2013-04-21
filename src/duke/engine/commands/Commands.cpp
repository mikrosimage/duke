#include "Commands.hpp"
#include <memory>

using namespace std;

namespace cmd {

CmdCmd::CmdCmd(const Commands &cmds) :
		cmds(cmds), pCommandDescription(nullptr) {
}

ManCmd::ManCmd(const Commands &cmds) :
		CmdCmd(cmds) {
}

std::string ManCmd::execute() {
	if (pCommandDescription)
		return pCommandDescription->help;
	return getDescription().help;
}

std::string ManCmd::doParseArguments(std::istream &stream) {
	string token;
	if (!(stream >> token))
		return {};
	pCommandDescription = cmds.find(token);
	if (!pCommandDescription)
		return "unknown command " + token;
	return {};
}

ArgsCmd::ArgsCmd(const Commands &cmds) :
		CmdCmd(cmds) {
}

std::string ArgsCmd::execute() {
	ostringstream oss;
	for (const auto arg : pCommandDescription->placeholders) {
		if (oss.tellp() != 0)
			oss << '\t';
		oss << getString(arg);
	}
	return oss.str();
}

std::string ArgsCmd::doParseArguments(std::istream &stream) {
	string token;
	if (!(stream >> token))
		return "you must provide an argument";
	pCommandDescription = cmds.find(token);
	if (!pCommandDescription)
		return "unknown command " + token;
	return {};
}

SuggestCmd::SuggestCmd(const Commands &cmds) :
		CmdCmd(cmds) {
}

std::string SuggestCmd::execute() {
	ostringstream oss;
	for (const auto &suggestion : cmds.suggest(value)) {
		if (oss.tellp() != 0)
			oss << '\t';
		oss << suggestion;
	}
	return oss.str();
}

std::string SuggestCmd::doParseArguments(std::istream &stream) {
	stream >> value;
	return {};
}

std::string LsCmd::execute() {
	return "listing of '" + path + "'";
}

std::string LsCmd::doParseArguments(std::istream &stream) {
	if (!(stream >> path))
		return string("'") + getString(CommandPlaceHolder::path) + "' expected";
	return {};
}

std::string NoOpCmd::execute() {
	return {};
}

FunctionCmd::FunctionCmd(const std::function<void()> &func) :
		m_Function(func) {
}

std::string FunctionCmd::execute() {
	m_Function();
	return {};
}

SuggestParam::SuggestParam(const Parameters &params) :
		params(params) {
}

std::string SuggestParam::execute() {
	ostringstream oss;
	for (const auto &suggestion : params.suggest(value)) {
		if (oss.tellp() != 0)
			oss << '\t';
		oss << suggestion;
	}
	return oss.str();
}

std::string SuggestParam::doParseArguments(std::istream &stream) {
	stream >> value;
	return {};
}
} // namespace cmd
