/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"ork/ork.hpp"

#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4371) //Boost layout changed
#pragma warning(disable:4619) //Boost disable non-existant warning
#pragma warning(disable:4625) //Boost implicit copy consructor
#pragma warning(disable:4626) //Boost implicit copy assignment
#pragma warning(disable:4668) //Boost+Windows undefined macro
#pragma warning(disable:5026) //Boost implicit move constructor
#pragma warning(disable:5027) //Boost implicit move assignment
#pragma warning(disable:5031) //Boost unmatched pragma push
#endif
#include"boost/program_options.hpp"
#if ORK_MSC
#pragma warning(pop)
#endif


#if ORK_UNICODE
#	define ORK_OVAL wvalue
#else
#	define ORK_OVAL value
#endif

namespace ork {


typedef int(*main_func)(const int argc, const char*const argv[]);
ORK_ORK_EXT(int) invoke_main(const std::vector<string>&args, main_func f);


namespace options = boost::program_options;

class command_handler {
protected:
private:
	string _desc_str;
	options::options_description _desc;
	options::variables_map _vm;
public:
	ORK_ORK_API command_handler();
	ORK_ORK_API virtual ~command_handler() {}
	ORK_NON_COPYABLE(command_handler)
public:
	//Configures and parses command line, calling add_options and extract_option_value
	ORK_ORK_API bool operator()(const int argc, const char_t*const argv[]);
	ORK_ORK_API bool operator()(const int argc, const char_t*const argv[], const bstring&positional_op);
	ORK_ORK_API bool operator()(const bstring&env_prefix);
	ORK_ORK_API bool operator()(std::istream&config_file);
	ORK_ORK_API const options::variables_map& variable_map()const {
		return _vm;
	}
private:
	void call_add_options();
	virtual void add_options(options::options_description&desc) = 0;//Template method for subclasses to add options
	virtual void extract_option_value(const bstring&option_name, string&option_value) {
		option_value = _vm[option_name].as<string>();
	}
	bool process_commands(const options::basic_parsed_options<char_t>&ops);
};


}//namespace ork
