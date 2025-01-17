#include <java_symbols.hpp>

#include <cstdint>

#include <algorithm>
#include <string_view>

extern "C" std::int32_t jurand_execute_v0(char* output, const char* fileroots, const char* names, const char* patterns, std::int64_t flags)
{
	auto arg_fileroots = std::vector<std::string_view>();
	arg_fileroots.reserve(8);
	
	auto parameters = Parameters();
	
	parameters.also_remove_annotations_ = flags & 1 << 0;
	parameters.in_place_ = flags & 1 << 1;
	parameters.strict_mode_ = flags & 1 << 2;
	
	for (std::string_view fileroot; fileroot = std::string_view(fileroots), not fileroot.empty(); fileroots += fileroot.size() + 1)
	{
		arg_fileroots.emplace_back(fileroot);
	}
	
	for (std::string_view name; name = std::string_view(names), not name.empty(); names += name.size() + 1)
	{
		parameters.names_.insert(name);
	}
	
	for (std::string_view pattern; pattern = std::string_view(patterns), not pattern.empty(); patterns += pattern.size() + 1)
	{
		parameters.patterns_.emplace_back(pattern, std::regex_constants::extended);
	}
	
	auto [code, ex_output] = java_symbols::execute(arg_fileroots, parameters);
	*std::ranges::copy(ex_output, output).out = '\0';
	
	return code;
}
