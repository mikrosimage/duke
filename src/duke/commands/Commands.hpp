#pragma once

#include <duke/commands/DescriptorSet.hpp>
#include <duke/commands/CommandDescription.hpp>
#include <utility>

namespace cmd {

struct Commands: public DescriptorSet<CommandDescription> {
	template<typename T>
	void addAndBind(CommandDescription && description) {
		add(std::move(description)).newInstanceFunc = []() {return new T();};
	}
	template<typename T, typename Arg>
	void addAndBind(CommandDescription && description, const Arg& arg) {
		add(std::move(description)).newInstanceFunc = [=]() {return new T(arg);};
	}

	Command* create(std::string token) const;

	std::string execute(const std::string &cmd) const;
};

} // namespace cmd
