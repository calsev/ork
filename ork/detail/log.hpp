/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#error This header can only be included from ork/common.hpp!
#endif

#include<memory>
#include"boost/log/sources/channel_logger.hpp"
#include"boost/log/sources/severity_channel_logger.hpp"
#include"boost/log/utility/manipulators/add_value.hpp"
#include"ork/detail/text.hpp"
#include"ork/detail/tools.hpp"
#include"ork/detail/file_system.hpp"


namespace ork {


enum class log_channel {
	debug_trace//For now all error, debug, trace goes to one file
	, output_data//For now, all program output goes to one file
};


enum class severity_level {
	trace
	, debug
	, info
	, warning
	, error
	, fatal
};


class logger {
public:
#if UNICODE
	typedef boost::log::sources::wchannel_logger_mt<log_channel> channel_source_type;
	typedef boost::log::sources::wseverity_channel_logger<severity_level, log_channel> severity_channel_source_type;
#else
	typedef boost::log::sources::channel_logger_mt<log_channel> channel_source_type;
	typedef boost::log::sources::severity_channel_logger<severity_level, log_channel> severity_channel_source_type;
#endif
protected:
	struct impl;
protected:
	std::unique_ptr<impl>p_impl;
public:
	logger(const file::path&log_file);
	~logger();
public:
	severity_channel_source_type&log();
	channel_source_type&lout();
	void flush_all();
};


//The logger must be terminated before main exits, so not a global!
extern logger g_log;
#define ORK_GLOBAL_LOG(LOG_DIRECTORY) ork::logger ork::g_log(LOG_DIRECTORY);


#define ORK_LOG_(LOG,SV) BOOST_LOG_SEV(LOG,SV)\
	<< boost::log::add_value(BTXT("Line"), LINE)\
	<< boost::log::add_value(BTXT("File"), FILEN)\
	<< boost::log::add_value(BTXT("Function"), FUNC)
#define ORK_LOG(SV) ORK_LOG_(ork::g_log.log(),SV)
#define ORK_LOUT BOOST_LOG(ork::g_log.lout())


#define ORK_LOC_BLOCK TXT("\n -- ") << FLOC << TXT(":\n -- ") << FUNC  << TXT("\n")

}//namespace ork
