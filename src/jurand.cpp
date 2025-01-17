#include <array>
#include <thread>
#include <utility>
#include <atomic>

#include <java_symbols.hpp>

using namespace java_symbols;

int main(int argc, const char** argv)
{
	auto args = std::span<const char*>(argv + 1, argc - 1);
	
	auto parameter_dict = parse_arguments(args, {"-a", "-i", "--in-place", "-s", "--strict"});
	
	if (parameter_dict.empty())
	{
		std::cout << 1 + (R"""(
Usage: jurand [optional flags] <matcher>... [file path]...
    Matcher:
        -n <name>
                simple (not fully-qualified) class name
        -p <pattern>
                regex pattern to match names used in code
        
    Optional flags:
        -a      also remove annotations used in code
        -i, --in-place
                replace the contents of files
        -s, --strict
                (wih -i only) fail if any of the specified options was redundant
                and no changes associated with the option were made

        -h, --help
                print help message
)""");
		return 0;
	}
	
	const auto parameters = interpret_args(parameter_dict);
	const auto fileroots = std::span<std::string_view>(parameter_dict.find("")->second);
	
	auto [exit_code, error_message] = execute(fileroots, parameters);
	
	if (exit_code != 0)
	{
		std::cout << error_message << "\n";
	}
	
	return exit_code;
}
