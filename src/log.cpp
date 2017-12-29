/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/

#include<fstream>
#include<iostream>

#include"ork/ork.hpp"
#include ORK_FILE_INCLUDE

#include"boost/core/null_deleter.hpp"
#include"boost/log/expressions.hpp"
#include"boost/log/sinks/sync_frontend.hpp"
#include"boost/log/sinks/text_ostream_backend.hpp"


namespace ork {

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
