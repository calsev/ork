/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#include<memory>
#include"boost/log/sources/channel_logger.hpp"
#include"boost/log/sources/severity_channel_logger.hpp"
#include"boost/log/utility/manipulators/add_value.hpp"


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
#if ORK_UNICODE
	typedef boost::log::sources::wchannel_logger_mt<log_channel> channel_source_type;
	typedef boost::log::sources::wseverity_channel_logger<severity_level, log_channel> severity_channel_source_type;
#else
	typedef boost::log::sources::channel_logger_mt<log_channel> channel_source_type;
	typedef boost::log::sources::severity_channel_logger<severity_level, log_channel> severity_channel_source_type;
#endif
private:
	struct impl;
private:
	std::unique_ptr<impl>p_impl;
public:
	ORK_ORK_API explicit logger(const file::path&log_file);
	ORK_ORK_API ~logger();
public:
	ORK_ORK_API severity_channel_source_type& log();
	ORK_ORK_API channel_source_type& lout();
	ORK_ORK_API void flush_all();
};


//The logger must be terminated before main exits, so not a global!
ORK_ORK_EXT(int) g_log(const string&directory);
ORK_ORK_EXT(logger&) g_log();
#define ORK_GLOBAL_LOG(LOG_DIRECTORY) const int g_log_val = ork::g_log(LOG_DIRECTORY);


#define ORK_LOG_(LOG_,SV) BOOST_LOG_SEV(LOG_,SV)\
	<< boost::log::add_value(BORK("Line"), ORK_LINE)\
	<< boost::log::add_value(BORK("File"), ORK_FILEN)\
	<< boost::log::add_value(BORK("Function"), ORK_FUNC)
#define ORK_LOG(SV) ORK_LOG_(ork::g_log().log(),SV)
#define ORK_LOUT BOOST_LOG(ork::g_log().lout())


#define ORK_LOC_BLOCK ORK("\n -- ") << ORK_FLOC << ORK(":\n -- ") << ORK_FUNC  << ORK("\n")

}//namespace ork
