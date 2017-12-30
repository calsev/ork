/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/

#include<fstream>
#include<iostream>
#include<mutex>

#include"ork/memory.hpp"
#include ORK_FILE_INCLUDE

#include"boost/core/null_deleter.hpp"
#include"boost/log/expressions.hpp"
#include"boost/log/sinks/sync_frontend.hpp"
#include"boost/log/sinks/text_ostream_backend.hpp"


namespace ork {


const ork::string debug_trace(ORK("debug_trace"));
const ork::string output_data(ORK("output_data"));

const ork::string&to_string(const log_channel val) {
	switch(val) {
	case log_channel::debug_trace:
		return debug_trace;
	case log_channel::output_data:
		return output_data;
	};
	ORK_UNREACHABLE
}
log_channel string2log_channel(const ork::string&str) {
	if(str == output_data) {
		return log_channel::output_data;
	}
	if(str == output_data) {
		return log_channel::output_data;
	}
	ORK_THROW(ORK("Invalid log_channel: ") << str);
}


const ork::string trace(ORK("trace"));
const ork::string debug(ORK("debug"));
const ork::string info(ORK("info"));
const ork::string warning(ORK("warning"));
const ork::string error(ORK("error"));
const ork::string fatal(ORK("fatal"));

const ork::string&to_string(const severity_level val) {
	switch(val) {
	case severity_level::trace:
		return trace;
	case severity_level::debug:
		return debug;
	case severity_level::info:
		return info;
	case severity_level::warning:
		return warning;
	case severity_level::error:
		return error;
	case severity_level::fatal:
		return fatal;
	};
	ORK_UNREACHABLE
}
severity_level string2severity_level(const ork::string&str) {
	if(str == trace) {
		return severity_level::trace;
	}
	if(str == debug) {
		return severity_level::debug;
	}
	if(str == info) {
		return severity_level::info;
	}
	if(str == warning) {
		return severity_level::warning;
	}
	if(str == error) {
		return severity_level::error;
	}
	if(str == fatal) {
		return severity_level::fatal;
	}
	ORK_THROW(ORK("Invalid severity_level: ") << str);
}


//This is little more than a synchronous wrapper for an ostream
template<class D>
class log_sink {
public:
	using stream_ptr = std::unique_ptr<o_stream, D>;
private:
	std::unique_ptr<o_stream, D>_stream;
	std::mutex _mutex;
public:
	log_sink(std::unique_ptr<o_stream, D>&&stream_) : _stream{ std::move(stream_) }, _mutex{ } {}
	ORK_MOVE_ONLY(log_sink)
public:
	void log(const string&message) {
		std::lock_guard<std::mutex>lock(_mutex);
		*_stream << message;
	}
};


struct log_stream::impl {
public:
	std::unique_ptr<o_stream>stream;
public:
	impl(std::unique_ptr<o_stream>&&stream_) :stream{ std::move(stream_) } {}
};

log_stream::log_stream(std::unique_ptr<o_stream>&&stream) : _pimpl{ new impl{ std::move(stream) } } {}
log_stream::~log_stream() {}

#define LOG_STREAM_OPERATOR(TYPE) \
log_stream& log_stream::operator<< (const TYPE val) {\
	*(_pimpl->stream) << val;\
	return *this;\
}
LOG_STREAM_OPERATOR(bool)
LOG_STREAM_OPERATOR(short)
LOG_STREAM_OPERATOR(unsigned short)
LOG_STREAM_OPERATOR(int)
LOG_STREAM_OPERATOR(unsigned)
LOG_STREAM_OPERATOR(long)
LOG_STREAM_OPERATOR(unsigned long)
LOG_STREAM_OPERATOR(long long)
LOG_STREAM_OPERATOR(unsigned long long)
LOG_STREAM_OPERATOR(float)
LOG_STREAM_OPERATOR(double)
LOG_STREAM_OPERATOR(long double)

#undef LOG_STREAM_OPERATOR
#define LOG_STREAM_OPERATOR(TYPE) \
log_stream& log_stream::operator<< (const TYPE val) {\
	*(_pimpl->stream) << ORK_BYTE_2_STR(val);\
	return *this;\
}

LOG_STREAM_OPERATOR(char)
LOG_STREAM_OPERATOR(char*)
LOG_STREAM_OPERATOR(bstring&)

#undef LOG_STREAM_OPERATOR
#define LOG_STREAM_OPERATOR(TYPE) \
log_stream& log_stream::operator<< (const TYPE val) {\
	*(_pimpl->stream) << ORK_WIDE_2_STR(val);\
	return *this;\
}

LOG_STREAM_OPERATOR(wchar_t)
LOG_STREAM_OPERATOR(wchar_t*)
LOG_STREAM_OPERATOR(wstring&)

log_stream& log_stream::operator<< (const void* val) {
	*(_pimpl->stream) << val;
	return *this;
}
log_stream& log_stream::operator<< (const std::streambuf* sb) {
	*(_pimpl->stream) << sb;
	return *this;
}
log_stream& log_stream::operator<< (std::ostream& (*pf)(std::ostream&)) {
	*(_pimpl->stream) << pf;
	return *this;
}
log_stream& log_stream::operator<< (std::ios& (*pf)(std::ios&)) {
	*(_pimpl->stream) << pf;
	return *this;
}
log_stream& log_stream::operator<< (std::ios_base& (*pf)(std::ios_base&)) {
	*(_pimpl->stream) << pf;
	return *this;
}


o_stream&operator<<(o_stream&strm, log_channel chan) {
	static const char_t* strings[] = {
		  ORK("Debug/Trace")
		, ORK("Output/Data")
	};

	const size_t index = static_cast<size_t>(chan);
	if(index < sizeof(strings) / sizeof(*strings)) strm << strings[index];
	else strm << index;

	return strm;
}


o_stream&operator<<(o_stream&strm, severity_level sev) {
	static const char_t* strings[] = {
		//These are fixed width, maybe this should be done in formatting instead
		  ORK("TRACE")
		, ORK("DEBUG")
		, ORK("INFO ")
		, ORK("WARN ")
		, ORK("ERROR")
		, ORK("FATAL")
	};

	const size_t index = static_cast<size_t>(sev);
	if(index < sizeof(strings) / sizeof(*strings)) strm << strings[index];
	else strm << index;

	return strm;
}


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, BORK("Severity"), severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, BORK("Channel"), log_channel)


namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;


#if ORK_UNICODE
typedef boost::log::wformatting_ostream formatting_ostream;
#else
typedef boost::log::formatting_ostream formatting_ostream;
#endif


boost::shared_ptr<o_stream>open_log(const file::path&file_name) {
	if(!file::ensure_directory(file_name))ORK_THROW(ORK("Could not create directory : ") << file_name)
		of_stream* p_stream = new of_stream();
	p_stream->open(file_name);//std::ios::app | std::ios::ate
	if(p_stream->fail())ORK_THROW(ORK("Error opening log : ") << file_name)
		//p_stream->rdbuf()->pubsetbuf(0, 0);//Less performance, more likely to catch error messages
		return boost::shared_ptr<o_stream>(p_stream);
}


void trace_formatter(const boost::log::record_view& rec, formatting_ostream& strm) {
	//Extract the Line attribute value that is added by the macro and put it into the stream
	const boost::log::value_ref<string> fullname = boost::log::extract<string>(BORK("File"), rec);
	const file::path fullpath(file::path(fullname.get()));
	string file(fullpath.filename().ORK_GEN_STR());
	file.resize(28, ORK(' '));
	string line(boost::log::extract<string>(BORK("Line"), rec).get());
	line.resize(4, ORK(' '));
	string function(boost::log::extract<string>(BORK("Function"), rec).get());
	function.resize(40, ORK(' '));

	//Simplified syntax is possible if attribute keywords are used.
	strm << ORK("[") << rec[severity] << ORK("]:");
	//Finally, put the record message to the stream
	strm << file << ORK("(") << line << ORK("):") << function << ORK("-- ") << rec[expr::message];
}


struct logger::impl {
public:
	typedef logger::channel_source_type channel_source_type;
	typedef logger::severity_channel_source_type severity_channel_source_type;
#if ORK_UNICODE
	typedef sinks::wtext_ostream_backend backend_type;
#else
	typedef sinks::text_ostream_backend backend_type;
#endif
	//This is not really a logging application so we do not need file features like rotation, etc.
	typedef sinks::synchronous_sink<backend_type> sink_type;
public:
	severity_channel_source_type trace;
	channel_source_type output;
	boost::shared_ptr<sink_type>p_trace_sink;
	boost::shared_ptr<sink_type>p_output_sink;
public:
	impl(const file::path&directory)
		: trace(boost::log::keywords::channel = log_channel::debug_trace)
		, output(boost::log::keywords::channel = log_channel::output_data) {
#if !ORK_STL_HAS_FILE
		/*
		Prevent crash on termination, see
		http://www.boost.org/doc/libs/1_60_0/libs/log/doc/html/log/rationale/why_crash_on_term.html
		*/
		file::path::codecvt();
#endif
		/*
		Create backends and for each attach both a console and file stream
		TODO: Some error handling for the files
		*/
		boost::shared_ptr<backend_type>trace_backend = boost::make_shared<backend_type>();
		trace_backend->add_stream(boost::shared_ptr<o_stream>(&ORK_CLOG, boost::null_deleter()));
		trace_backend->add_stream(open_log(directory / ORK("trace.log")));
		trace_backend->auto_flush(true);//Enable auto-flushing because we are using this for errors
										//Wrap it into a mutithreaded frontend
										//boost::shared_ptr<sink_type> trace_sink(new sink_type(trace_backend));
		p_trace_sink.reset(new sink_type(trace_backend));
		p_trace_sink->set_filter((
			expr::has_attr(channel) && channel == log_channel::debug_trace
#if !ORK_DEBUG && 0
			&& expr::has_attr(severity) && severity != severity_level::trace
#endif
			));
		//Add some standard formatting at the beginning
		p_trace_sink->set_formatter(&trace_formatter);


		boost::shared_ptr<backend_type>output_backend = boost::make_shared<backend_type>();
		output_backend->add_stream(boost::shared_ptr<o_stream>(&ORK_COUT, boost::null_deleter()));
		output_backend->add_stream(open_log(directory / ORK("output.log")));
		//Wrap it into a mutithreaded frontend
		//boost::shared_ptr<sink_type> output_sink(new sink_type(output_backend));
		p_output_sink.reset(new sink_type(output_backend));
		p_output_sink->set_filter((expr::has_attr(channel) && channel == log_channel::output_data));


		//Register with the core so that logging output can begin
		boost::shared_ptr<boost::log::core>core = boost::log::core::get();
		core->add_sink(p_trace_sink);
		core->add_sink(p_output_sink);
	}
	void flush_all() {
		p_trace_sink->flush();
		p_output_sink->flush();
	}
};


logger::logger(const file::path&log_directory) : p_impl(new impl(log_directory)) {}


//The logger must be terminated before main exits, so not a global!
logger::~logger() {
	/*
	Prevent crash on termination, see
	http://www.boost.org/doc/libs/1_60_0/libs/log/doc/html/log/rationale/why_crash_on_term.html
	*/
	boost::log::core::get()->remove_all_sinks();
}


logger::severity_channel_source_type& logger::log() {
	return p_impl->trace;
}


logger::channel_source_type& logger::lout() {
	return p_impl->output;
}

void logger::flush_all() {
	p_impl->flush_all();
}


logger*_g_log = nullptr;
int g_log(const string&directory) {
	static logger log(directory);
	_g_log = &log;
	return 0;
}
logger& g_log() {
	return *_g_log;
}


}//namespace ork
