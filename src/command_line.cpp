/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<string>
#include<sstream>
#include"boost/program_options.hpp"
#include"ork/command_Line.hpp"

namespace ork {

int invoke_main(const std::vector<string>&args, main_func f) {
	std::vector<const letr*const>argv;
	argv.push_back(TXT("this_should_be_the_invoked_command"));
	for(const string&arg : args)argv.push_back(arg.c_str());

	string_stream cmd;
	for(const letr*const arg : argv) {
		cmd << arg << TXT(" ");
	}
	LOG(severity_level::info) << TXT("\n -- Command Line: ") << cmd.str();

	return f(static_cast<int>(argv.size()), argv.data());
}

command_handler::command_handler()
	: _desc_str()//Initialized by call_add_options
	, _desc(BTXT("Standard Options"))
	, _vm() {}


void command_handler::call_add_options() {
	_desc.add_options()
		(BTXT("help,h"), BTXT("Produce help message"));//Short option aliases do not work until notify, so we can't use them for help
	add_options(_desc);

	b_string_stream desc_byte;
	desc_byte << _desc;//string operation only defined for std::ostream
	_desc_str = BYTE_2_STR(desc_byte.str());
}


bool command_handler::process_commands(const options::basic_parsed_options<letr>&ops) {
	try {
		options::store(ops, _vm);
		if(_vm.count(BTXT("help"))) {//Check here to ignore any missing options
			LOG(severity_level::info) << TXT('\n') << _desc_str << TXT('\n');
			return false;
		}
		options::notify(_vm);//Exceptions for missing required options raised here

		string_stream options;
		options << TXT("\n -- Command Line Options:");
		for(auto&option : _desc.options()) {
			const bstring option_name = option->long_name();
			string option_value = TXT("Unspecified");
			if(_vm.count(option_name)) {
				extract_option_value(option_name, option_value);
			}
			options << TXT("\n  - ") << BYTE_2_STR(option_name) << TXT(": ") << option_value;
		}
		LOG(severity_level::info) << options.str();
	}
	catch(options::error &e) {
		LOG(severity_level::error) << TXT("\nProblem storing command line options:\n  - ") << BYTE_2_STR(e.what());
		LOG(severity_level::error) << TXT('\n') << _desc_str << TXT('\n');
		return false;
	}

	return true;
}

#define COMMAND_CATCH \
	catch(options::error &e) {\
		LOG(severity_level::error) << TXT("\nProblem parsing command line options:\n  - ") << BYTE_2_STR(e.what());\
		LOG(severity_level::error) << TXT('\n') << _desc_str << TXT('\n');\
		return false;\
	}


bool command_handler::operator()(const int argc, const letr*const argv[]) {
	try {
		call_add_options();
		options::basic_parsed_options<letr>ops(options::parse_command_line<letr>(argc, argv, _desc));
		return process_commands(ops);
	}
	COMMAND_CATCH
}


bool command_handler::operator()(const int argc, const letr*const argv[], const bstring&positional_op) {
	try {
		call_add_options();
		options::positional_options_description p;
		p.add(positional_op.c_str(), -1);
		options::basic_parsed_options<letr>ops(options::command_line_parser(argc, argv).options(_desc).positional(p).run());
		return process_commands(ops);
	}
	COMMAND_CATCH
}


bool command_handler::operator()(const bstring&env_prefix) {
	try {
		call_add_options();
		options::basic_parsed_options<letr>ops(options::parse_environment(_desc, env_prefix));
		return process_commands(ops);
	}
	COMMAND_CATCH
}


bool command_handler::operator()(std::istream&config_file) {
	try {
		call_add_options();
		options::basic_parsed_options<letr>ops(options::parse_config_file(config_file, _desc));
		return process_commands(ops);
	}
	COMMAND_CATCH
}

}//namespace ork
