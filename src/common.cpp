/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"boost/algorithm/string.hpp"
#include"boost/core/null_deleter.hpp"
#include"boost/filesystem/path.hpp"
#include"boost/filesystem/fstream.hpp"
#include"boost/filesystem.hpp"
#include"boost/log/expressions.hpp"
#include"boost/log/sinks/sync_frontend.hpp"
#include"boost/log/sinks/text_ostream_backend.hpp"

#include"ork/ork.hpp"
#include"ork/command_line.hpp"
#include"ork/tagger.hpp"
#include"ork/file_utils.hpp"

#define NOMINMAX
#include"windows.h"
#include"DbgHelp.h"


namespace ork {


/*
This is the combined implementation file for the common library.
We do it this way because the library is compiled for VS2012 also, and we want it to be easy to include in a project.
It is getting a bit silly, and we might want to instead just have projects targeting different platforms.
*/

string to_string(const bool val) {
	return val ? TXT("true") : TXT("false");
}
bool string2bool(string val) {
	boost::to_lower(val);
	if(val == TXT("t"))return true;
	if(val == TXT("true"))return true;
	if(val == TXT("1"))return true;
	if(val == TXT("f"))return false;
	if(val == TXT("false"))return false;
	if(val == TXT("0"))return false;
	ORK_THROW(TXT("Invalid bool value"));
}



/*
From database.hpp
*/



bstring uuid2string(const char*const value) {
	b_string_stream stream;
	stream << std::hex;
	LOOPJ(16) {
		stream << (value[j] & (char(15) << 4));
		stream << (value[j] & char(15));
	}
	return stream.str();
}
std::vector<char> string2uuid(const bstring&val) {
	std::vector<char> retval;
	b_string_stream stream;
	stream << std::hex << val;
	LOOPJ(16) {
		char upper, lower;
		stream >> upper >> lower;
		retval.push_back((upper << 4) | lower);
	}
	return retval;
}


/*
From exception.hpp
*/

#ifdef _MSC_VER

backtrace generate_backtrace(const unsigned skip_frames) {
	static const unsigned capture_frames = 128;
	static const unsigned max_name_len = 1024;

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, TRUE);

	void *addrs[capture_frames];
	const unsigned short num_addr = CaptureStackBackTrace(skip_frames + 1, capture_frames, addrs, nullptr);

	// Oddly allocated, I know. You must allocate additional memory after the
	// end of the SYMBOL_INFO structure to store the name. This memory must be
	// contiguous, so we can't use basic "new" statements, must resort to malloc
	SYMBOL_INFO *symbol = static_cast<SYMBOL_INFO *>(malloc(sizeof(SYMBOL_INFO) + sizeof(char) * max_name_len));
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = max_name_len - 1;

	// This one stores file/line information
	IMAGEHLP_LINE64 line;
	line.SizeOfStruct = sizeof(line);

	backtrace result;

	LOOPI(num_addr) {
		DWORD64 addr = reinterpret_cast<DWORD64>(addrs[i]);

		DWORD displacement; // I don't care about this output parameter, but it must be supplied
		SymFromAddr(process, addr, 0, symbol);
		SymGetLineFromAddr64(process, addr, &displacement, &line);

		stack_frame frame;
		frame.function = symbol->Name;
		frame.filename = line.FileName;
		frame.line = line.LineNumber;
		result.frames.push_back(frame);
	}

	free(symbol);
	return result;
}


bstring backtrace::str() const {
	b_string_stream stream;

	stream << BTXT("backtrace:\n");

	unsigned index = 0;
	std::deque<stack_frame>::const_iterator frame;
	for(const stack_frame&frame : frames) {
		stream << index++ << BTXT(": ") << frame.function << BTXT(" (") << frame.filename << BTXT(":") << frame.line << BTXT(")\n");
	}

	return stream.str();
}

#else//_MSC_VER

backtrace generate_backtrace(const unsigned skip_frames) {
	return backtrace();
}


bstring backtrace::str() const {
	return bstring();
}

#endif//_MSC_VER

std::ostream &operator << (std::ostream &stream, const backtrace &trace) {
	return stream << trace.str();
}


/*
From file_system.hpp
*/

bool test_directory(const file::path&file_or_directory) {
	return file::exists(file_or_directory) && file::is_directory(file_or_directory);
}
bool test_file(const file::path&file) {
	return file::exists(file) && file::is_regular_file(file);
}
bool ensure_directory(file::path file_or_directory) {
	if(!file::is_directory(file_or_directory))file_or_directory.remove_filename();
	if(file_or_directory.empty() || file::exists(file_or_directory))return true;
	return file::create_directories(file_or_directory);
}
bool ensure_file(const file::path&file) {
	if(!ensure_directory(file))return false;
	if(file.empty() || file::exists(file))return true;
	if(!file::is_regular_file(file))return false;
	return file::create_directories(file);
}


/*
From file_utils.hpp
*/
bool top_subdirectory(const file::path&dir, file::path&p) {
	if(!test_directory(dir))FILE_OPEN_ERR(TXT("Error searching directory!"), dir);

	//Create a list of the paths in the directory, and then sort it
	std::vector<file::path>paths;
	std::copy(file::directory_iterator(dir), file::directory_iterator(), std::back_inserter(paths));
	std::sort(paths.begin(), paths.end());

	for(const file::path&curr_path : paths) {
		if(file::is_directory(curr_path)) {
			p = curr_path;
			return true;
		}
	}

	return false;
}



/*
From log.hpp
*/

o_stream&operator<<(o_stream&strm, log_channel chan) {
	static const letr* strings[] = {
		TXT("Debug/Trace")
		, TXT("Output/Data")
	};

	const size_t index = static_cast<size_t>(chan);
	if(index < sizeof(strings) / sizeof(*strings)) strm << strings[index];
	else strm << index;

	return strm;
}


o_stream&operator<<(o_stream&strm, severity_level sev) {
	static const letr* strings[] = {
		//These are fixed width, maybe this should be done in formatting instead
		TXT("TRACE")
		, TXT("DEBUG")
		, TXT("INFO ")
		, TXT("WARN ")
		, TXT("ERROR")
		, TXT("FATAL")
	};

	const size_t index = static_cast<size_t>(sev);
	if(index < sizeof(strings) / sizeof(*strings)) strm << strings[index];
	else strm << index;

	return strm;
}


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, BTXT("Severity"), severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, BTXT("Channel"), log_channel)


namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;


#if UNICODE
typedef boost::log::wformatting_ostream formatting_ostream;
#else
typedef boost::log::formatting_ostream formatting_ostream;
#endif


boost::shared_ptr<o_stream>open_log(const file::path&file_name) {
	if(!ensure_directory(file_name))ORK_THROW(TXT("Could not create directory : ") << file_name)
		of_stream* p_stream = new of_stream();
	p_stream->open(file_name);//std::ios::app | std::ios::ate
	if(p_stream->fail())ORK_THROW(TXT("Error opening log : ") << file_name)
		//p_stream->rdbuf()->pubsetbuf(0, 0);//Less performance, more likely to catch error messages
		return boost::shared_ptr<o_stream>(p_stream);
}


void trace_formatter(const boost::log::record_view& rec, formatting_ostream& strm) {
	//Extract the Line attribute value that is added by the macro and put it into the stream
	const boost::log::value_ref<string> fullname = boost::log::extract<string>(BTXT("File"), rec);
	const file::path fullpath(file::path(fullname.get()));
	string file(fullpath.filename().GEN_STR());
	file.resize(28, TXT(' '));
	string line(boost::log::extract<string>(BTXT("Line"), rec).get());
	line.resize(4, TXT(' '));
	string function(boost::log::extract<string>(BTXT("Function"), rec).get());
	function.resize(40, TXT(' '));

	//Simplified syntax is possible if attribute keywords are used.
	strm << TXT("[") << rec[severity] << TXT("]:");
	//Finally, put the record message to the stream
	strm << file << TXT("(") << line << TXT("):") << function << TXT("-- ") << rec[expr::message];
}


struct logger::impl {
public:
	typedef logger::channel_source_type channel_source_type;
	typedef logger::severity_channel_source_type severity_channel_source_type;
#if UNICODE
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
		/*
		Prevent crash on termination, see
		http://www.boost.org/doc/libs/1_60_0/libs/log/doc/html/log/rationale/why_crash_on_term.html
		*/
		file::path::codecvt();

		/*
		Create backends and for each attach both a console and file stream
		TODO: Some error handling for the files
		*/
		boost::shared_ptr<backend_type>trace_backend = boost::make_shared<backend_type>();
		trace_backend->add_stream(boost::shared_ptr<o_stream>(&CLOG, boost::null_deleter()));
		trace_backend->add_stream(open_log(directory / TXT("trace.log")));
		trace_backend->auto_flush(true);//Enable auto-flushing because we are using this for errors
		//Wrap it into a mutithreaded frontend
		//boost::shared_ptr<sink_type> trace_sink(new sink_type(trace_backend));
		p_trace_sink.reset(new sink_type(trace_backend));
		p_trace_sink->set_filter((
			expr::has_attr(channel) && channel == log_channel::debug_trace
#if !defined(_DEBUG) && !defined(DEBUG)
			&& expr::has_attr(severity) && severity != severity_level::trace
#endif
			));
		//Add some standard formatting at the beginning
		p_trace_sink->set_formatter(&trace_formatter);


		boost::shared_ptr<backend_type>output_backend = boost::make_shared<backend_type>();
		output_backend->add_stream(boost::shared_ptr<o_stream>(&COUT, boost::null_deleter()));
		output_backend->add_stream(open_log(directory / TXT("output.log")));
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



/*
From text.hpp
*/

#if UNICODE || 1
string_converter_type&g_string_converter() {//From string_types.hpp
	static string_converter_type s_convert;
	return s_convert;
}
#endif


}//namespace ork
