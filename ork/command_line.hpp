/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"boost/program_options/variables_map.hpp"
#include"boost/program_options/options_description.hpp"
#include"ork/ork.hpp"

namespace options = boost::program_options;


#ifdef UNICODE
#define OVAL wvalue
#else
#define OVAL value
#endif

namespace ork {


typedef int(*main_func)(const int argc, const char*const argv[]);
int invoke_main(const std::vector<string>&args, main_func f);


class command_handler {
protected:
private:
	string _desc_str;
	options::options_description _desc;
	options::variables_map _vm;
public:
	command_handler();
	virtual ~command_handler() {}
public:
	//Configures and parses command line, calling add_options and extract_option_value
	bool operator()(const int argc, const letr*const argv[]);
	bool operator()(const int argc, const letr*const argv[], const bstring&positional_op);
	bool operator()(const bstring&env_prefix);
	bool operator()(std::istream&config_file);
	const options::variables_map&variable_map()const {
		return _vm;
	}
private:
	void call_add_options();
	virtual void add_options(options::options_description&desc) = 0;//Template method for subclasses to add options
	virtual void extract_option_value(const bstring&option_name, string&option_value) {
		option_value = _vm[option_name].as<string>();
	}
	bool process_commands(const options::basic_parsed_options<letr>&ops);
};


}//namespace ork
