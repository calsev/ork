/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/

#include<fstream>
#include<iostream>
#include<mutex>
#include<sstream>

#include"ork/memory.hpp"


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


//This is little more than a synchronous wrapper around an o_stream
class log_stream {
public:
	using stream_ptr = std::shared_ptr<o_stream>;
private:
	stream_ptr _stream;
	std::mutex _mutex;
public:
	explicit log_stream(stream_ptr stream_) : _stream{stream_}, _mutex{} {}
	ORK_NON_COPYABLE(log_stream)
public:
	void log(const string&message) {
		std::lock_guard<std::mutex>lock(_mutex);
		*_stream << message;
	}
	void flush() {
		_stream->flush();
	}
};


class log_sink {
public:
	using stream_ptr = std::shared_ptr<log_stream>;
private:
	std::vector<stream_ptr>_streams = {};
	bool _auto_flush = false;
public:
	log_sink() {}
	log_sink(const bool auto_flush) : _auto_flush{auto_flush} {}
public:
	void insert(const stream_ptr&ptr) {
		_streams.push_back(ptr);
	}
	void log(const string&message) {
		for(auto&stream : _streams) {
			stream->log(message);
			if(_auto_flush) {
				stream->flush();
			}
		}
	}
	void set_auto_flush(const bool auto_flush) {
		_auto_flush = auto_flush;
	}
	void flush() {
		for(auto&stream : _streams) {
			stream->flush();
		}
	}
};


std::shared_ptr<log_stream>open_file_log_stream(const file::path&file_name) {
	if(!file::ensure_directory(file_name)) {
		ORK_THROW(ORK("Could not create directory : ") << file_name.ORK_GEN_STR())
	}
	of_stream* p_stream = new of_stream();
	p_stream->open(file_name);//std::ios::app | std::ios::ate
	if(p_stream->fail()) {
		ORK_THROW(ORK("Error opening log : ") << file_name)
	}
	//p_stream->rdbuf()->pubsetbuf(0, 0);//Less performance, more likely to catch error messages
	return std::shared_ptr<log_stream>(new log_stream(log_stream::stream_ptr(p_stream)));
}


//This is where our logging system falls short of a generic filter system; try to hide it somewhat in one place
class log_multiplexer {
private:
	std::array<log_sink, severity_levels.size()>_severity_sinks = {};
	log_sink _data_sink = {};
public:
	log_multiplexer(const file::path&root_directory){
		auto lout = log_sink::stream_ptr{new log_stream{log_stream::stream_ptr{&ORK_CLOG, singleton_deleter<o_stream>()}}};
		auto lerr = log_sink::stream_ptr{new log_stream{log_stream::stream_ptr{&ORK_CERR, singleton_deleter<o_stream>()}}};
		auto flog = open_file_log_stream(root_directory / ORK("trace.log"));
		auto fdata = open_file_log_stream(root_directory / ORK("output.log"));

		for(const auto sv : severity_levels) {
			auto sink = _severity_sinks[static_cast<size_t>(sv)];
			if(sv < severity_level::error) {
				sink.insert(lout);
				sink.set_auto_flush(true);
			}
			else {
				sink.insert(lerr);
			}
			sink.insert(flog);
		}
		_data_sink.insert(lout);
		_data_sink.insert(fdata);
	}
public:
	void log(const log_channel channel, const severity_level severity, const o_string_stream&stream) {
		const string message = stream.str();
		switch(channel) {
		case log_channel::debug_trace:
			log_severity(severity, message);
			break;
		case log_channel::output_data:
			_data_sink.log(stream.str());
			break;
		};
		ORK_UNREACHABLE
	}
	void flush_all() {
		for(auto&sink : _severity_sinks) {
			sink.flush();
		}
		_data_sink.flush();
	}
private:
	void log_severity(const severity_level severity, const string&message) {
		const bool do_it = ORK_DEBUG || severity > severity_level::debug;
		if(do_it) {//To avoid constant conditional expression
			_severity_sinks[static_cast<size_t>(severity)].log(message);
		}
	}
};


struct log_scope::impl {
public:
	std::shared_ptr<log_multiplexer>multiplexer;
	log_channel channel;
	severity_level severity;
	o_string_stream stream;
public:
	impl(std::shared_ptr<log_multiplexer>&mp, const log_channel lc, const severity_level sv)
		: multiplexer{mp}
		, channel{lc}
		, severity{sv}
		, stream{}
	{}
	~impl() {
		multiplexer->log(channel, severity, stream);
	}
	ORK_MOVE_ONLY(impl)
};

log_scope::log_scope(std::unique_ptr<impl>&&ptr) : _pimpl{ std::move(ptr) } {}
log_scope::~log_scope() {}

#define LOG_STREAM_OPERATOR(TYPE) \
log_scope& log_scope::operator<< (const TYPE val) {\
	_pimpl->stream << val;\
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
log_scope& log_scope::operator<< (const TYPE val) {\
	_pimpl->stream << ORK_BYTE_2_STR(val);\
	return *this;\
}

LOG_STREAM_OPERATOR(char)
LOG_STREAM_OPERATOR(char*)
LOG_STREAM_OPERATOR(bstring&)

#undef LOG_STREAM_OPERATOR
#define LOG_STREAM_OPERATOR(TYPE) \
log_scope& log_scope::operator<< (const TYPE val) {\
	_pimpl->stream << ORK_WIDE_2_STR(val);\
	return *this;\
}

LOG_STREAM_OPERATOR(wchar_t)
LOG_STREAM_OPERATOR(wchar_t*)
LOG_STREAM_OPERATOR(wstring&)

log_scope& log_scope::operator<< (const void* val) {
	_pimpl->stream << val;
	return *this;
}
log_scope& log_scope::operator<< (const std::streambuf* sb) {
	_pimpl->stream << sb;
	return *this;
}
log_scope& log_scope::operator<< (std::ostream& (*pf)(std::ostream&)) {
	_pimpl->stream << pf;
	return *this;
}
log_scope& log_scope::operator<< (std::ios& (*pf)(std::ios&)) {
	_pimpl->stream << pf;
	return *this;
}
log_scope& log_scope::operator<< (std::ios_base& (*pf)(std::ios_base&)) {
	_pimpl->stream << pf;
	return *this;
}


const string&to_formatted_string(const severity_level sv) {
	static const std::array<const string, severity_levels.size()>strings = {{
		  ORK("TRACE")
		, ORK("DEBUG")
		, ORK("INFO ")
		, ORK("WARN ")
		, ORK("ERROR")
		, ORK("FATAL")
	}};
	return strings[static_cast<size_t>(sv)];
}


struct logger::impl {
public:
	file::path root_directory;
	std::shared_ptr<log_multiplexer>multiplexer;
public:
	impl(const file::path&directory)
		: root_directory(directory)
		, multiplexer{new log_multiplexer(directory)}
	{}
	void flush_all() {
		multiplexer->flush_all();
	}
};


logger::logger(const file::path&log_directory) : _pimpl(new impl(log_directory)) {}
logger::~logger() {}

const file::path& logger::root_directory() {
	return _pimpl->root_directory;
}

log_scope logger::get_log_scope(
	  const string&file_
	, const string&line_
	, const string&function_
	, const log_channel channel
	, const severity_level severity)
{
	const file::path fullpath(file_);
	string file(fullpath.filename().ORK_GEN_STR());
	file.resize(28, ORK(' '));

	string line(line_);
	line.resize(4, ORK(' '));

	string function(function_);
	function.resize(40, ORK(' '));

	std::unique_ptr<log_scope::impl> ls_impl(new log_scope::impl(_pimpl->multiplexer, channel, severity));
	log_scope scope(std::move(ls_impl));
	//Output formatted context first
	scope << ORK("[") << to_formatted_string(severity) << ORK("]:") << file << ORK("(") << line << ORK("):") << function << ORK("-- ");

	//Finally, return the stream to the client
	return std::move(scope);
}

void logger::flush_all() {
	_pimpl->flush_all();
}



logger*_g_log = nullptr;
int make_global_log(const string&directory) {
	static logger log(directory);
	_g_log = &log;
	return 0;
}
logger& get_global_log() {
	return *_g_log;
}


}//namespace ork
