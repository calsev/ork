/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#define ORK_STL_INC_FILE <string>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include"ork/core/stl_include.inl"

#include"ork/command_Line.hpp"


namespace ork {

int invoke_main(const std::vector<string>&args, main_func f) {
	std::vector<const char_t*>argv;
	argv.push_back(ORK("this_should_be_the_invoked_command"));
	for(const string&arg : args)argv.push_back(arg.c_str());

	string_stream cmd;
	for(const char_t*const arg : argv) {
		cmd << arg << ORK(" ");
	}
	ORK_LOG(severity_level::info) << ORK("\n -- Command Line: ") << cmd.str();

	return f(static_cast<int>(argv.size()), argv.data());
}

command_handler::command_handler()
	: _desc_str()//Initialized by call_add_options
	, _desc(BORK("Standard Options"))
	, _vm() {}


void command_handler::call_add_options() {
	_desc.add_options()
		(BORK("help,h"), BORK("Produce help message"));//Short option aliases do not work until notify, so we can't use them for help
	add_options(_desc);

	b_string_stream desc_byte;
	desc_byte << _desc;//string operation only defined for std::ostream
	_desc_str = ORK_BYTE_2_STR(desc_byte.str());
}


bool command_handler::process_commands(const options::basic_parsed_options<char_t>&ops) {
	try {
		options::store(ops, _vm);
		if(_vm.count(BORK("help"))) {//Check here to ignore any missing options
			ORK_LOG(severity_level::info) << ORK('\n') << _desc_str << ORK('\n');
			return false;
		}
		options::notify(_vm);//Exceptions for missing required options raised here

		string_stream options;
		options << ORK("\n -- Command Line Options:");
		for(auto&option : _desc.options()) {
			const bstring option_name = option->long_name();
			string option_value = ORK("Unspecified");
			if(_vm.count(option_name)) {
				extract_option_value(option_name, option_value);
			}
			options << ORK("\n  - ") << ORK_BYTE_2_STR(option_name) << ORK(": ") << option_value;
		}
		ORK_LOG(severity_level::info) << options.str();
	}
	catch(options::error &e) {
		ORK_LOG(severity_level::error) << ORK("\nProblem storing command line options:\n  - ") << ORK_BYTE_2_STR(e.what());
		ORK_LOG(severity_level::error) << ORK('\n') << _desc_str << ORK('\n');
		return false;
	}

	return true;
}

#define COMMAND_CATCH \
	catch(options::error &e) {\
		ORK_LOG(severity_level::error) << ORK("\nProblem parsing command line options:\n  - ") << ORK_BYTE_2_STR(e.what());\
		ORK_LOG(severity_level::error) << ORK('\n') << _desc_str << ORK('\n');\
		return false;\
	}


bool command_handler::operator()(const int argc, const char_t*const argv[]) {
	try {
		call_add_options();
		options::basic_parsed_options<char_t>ops(options::parse_command_line<char_t>(argc, argv, _desc));
		return process_commands(ops);
	}
	COMMAND_CATCH
}


#if ORK_UNICODE
using command_line_parser = options::basic_command_line_parser<wchar_t>;
#else
using command_line_parser = options::basic_command_line_parser<char>;
#endif


bool command_handler::operator()(const int argc, const char_t*const argv[], const bstring&positional_op) {
	try {
		call_add_options();
		options::positional_options_description p;
		p.add(positional_op.c_str(), -1);
		options::basic_parsed_options<char_t>ops(command_line_parser(argc, argv).options(_desc).positional(p).run());
		return process_commands(ops);
	}
	COMMAND_CATCH
}


bool command_handler::operator()(const bstring&env_prefix) {
	try {
		call_add_options();
		options::basic_parsed_options<char_t>ops(options::parse_environment(_desc, env_prefix));
		return process_commands(ops);
	}
	COMMAND_CATCH
}


bool command_handler::operator()(std::istream&config_file) {
	try {
		call_add_options();
		options::basic_parsed_options<char_t>ops(options::parse_config_file(config_file, _desc));
		return process_commands(ops);
	}
	COMMAND_CATCH
}

}//namespace ork
