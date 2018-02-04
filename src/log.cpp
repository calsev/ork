/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/ork.hpp"
#define ORK_STL_INC_FILE <fstream>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <iostream>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <mutex>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include "ork/core/stl_include.inl"

#include "ork/memory.hpp"


namespace ork {


const ork::string debug_trace(ORK("debug_trace"));
const ork::string output_data(ORK("output_data"));

const ork::string& to_string(const log_channel val)
{
    switch(val) {
        case log_channel::debug_trace:
            return debug_trace;
        case log_channel::output_data:
            return output_data;
    };
    ORK_UNREACHABLE
}
log_channel string2log_channel(const ork::string& str)
{
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

const ork::string& to_string(const severity_level val)
{
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
severity_level string2severity_level(const ork::string& str)
{
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


/*
This is little more than a wrapper around an o_stream
Access is serialized by message_guard
*/
class log_stream {
public:
    using stream_ptr = std::shared_ptr<o_stream>;

private:
    stream_ptr _stream;

public:
    explicit log_stream(stream_ptr stream_)
        : _stream{stream_}
    {}
    ~log_stream()
    {
        flush();
    }
    ORK_NON_COPYABLE(log_stream);

public:
    void log(const string& message)
    {
        *_stream << message << ORK('\n');
    }
    void flush()
    {
        _stream->flush();
    }
};


class log_sink {
public:
    using stream_ptr = std::shared_ptr<log_stream>;

private:
    std::vector<stream_ptr> _streams = {};
    bool _auto_flush = false;

public:
    log_sink() {}
    log_sink(const bool auto_flush)
        : _auto_flush{auto_flush}
    {}
    ORK_NON_COPYABLE(log_sink);

public:
    void insert(const stream_ptr& ptr)
    {
        _streams.push_back(ptr);
    }
    void log(const string& message)
    {
        for(auto& stream : _streams) {
            stream->log(message);
            if(_auto_flush) {
                stream->flush();
            }
        }
    }
    void set_auto_flush(const bool auto_flush)
    {
        _auto_flush = auto_flush;
    }
    void flush()
    {
        for(auto& stream : _streams) {
            stream->flush();
        }
    }
};


std::shared_ptr<log_stream> open_file_log_stream(const file::path& file_name)
{
    if(!file::ensure_directory(file_name)) {
        ORK_THROW(ORK("Could not create directory : ") << file_name.ORK_GEN_STR())
    }
    of_stream* p_stream = new of_stream();
    p_stream->open(file_name); // std::ios::app | std::ios::ate
    if(p_stream->fail()) {
        ORK_THROW(ORK("Error opening log : ") << file_name)
    }
    // p_stream->rdbuf()->pubsetbuf(0, 0);//Less performance, more likely to catch error messages
    return std::shared_ptr<log_stream>(new log_stream(log_stream::stream_ptr(p_stream)));
}


class message_guard {
private:
    ork::string _message = {};
    int _stage = 0;
    mutable std::mutex _mutex = {};
    log_channel _channel;
    severity_level _severity;

public:
    message_guard(const log_channel lc, const severity_level sv)
        : _channel{lc}
        , _severity{sv}
    {}
    ORK_NON_COPYABLE(message_guard);

public:
    log_channel channel() const
    {
        return _channel;
    }
    severity_level severity() const
    {
        return _severity;
    }

    // This should be called to check if the message is ready to be cleared
    bool done() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _stage > 0;
    }
    // This should be called once to complete the message
    void log(const o_string_stream& stream)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_stage != 0) {
            ORK_THROW(ORK("Message pump called in wrong order"));
        }
        _message = stream.str();
        _stage = 1;
    }
    // This should be called once after completion
    ork::string clear_message()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_stage != 1) {
            ORK_THROW(ORK("Message pump called in wrong order"));
        }
        _stage = 2;
        return std::move(_message);
    }
};
using guard_ptr = std::shared_ptr<message_guard>;


// This is where our logging system falls short of a generic filter system; try to hide it somewhat in one place
class log_multiplexer {
private:
    using sink_ptr = std::unique_ptr<log_sink>;

private:
    std::vector<sink_ptr> _severity_sinks = {};
    log_sink _data_sink = {};
    std::vector<guard_ptr> _messages = {};
    size_t _message_index = 0;
    std::mutex _mutex = {};

public:
    log_multiplexer(const file::path& root_directory)
    {
        auto lout = log_sink::stream_ptr{new log_stream{
            log_stream::stream_ptr{&ORK_CLOG, singleton_deleter<o_stream>()}}};
        auto lerr = log_sink::stream_ptr{new log_stream{
            log_stream::stream_ptr{&ORK_CERR, singleton_deleter<o_stream>()}}};
        auto flog = open_file_log_stream(root_directory / ORK("trace.log"));
        auto fdata = open_file_log_stream(root_directory / ORK("output.log"));

        for(const auto sv : severity_levels) {
            sink_ptr sink{};
            if(sv < severity_level::error) {
                sink.reset(new log_sink{false});
                sink->insert(lout);
            }
            else {
                sink.reset(new log_sink{true});
                sink->insert(lerr);
            }
            sink->insert(flog);
            _severity_sinks.emplace_back(std::move(sink));
        }
        _data_sink.insert(lout);
        _data_sink.insert(fdata);
    }
    ORK_NON_COPYABLE(log_multiplexer);

public:
    guard_ptr get_message_guard(const log_channel lc, const severity_level sv)
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _messages.emplace_back(new message_guard{lc, sv});
        return _messages.back();
    }
    void on_scope_exit()
    {
        std::lock_guard<std::mutex> lock{_mutex};
        while(_message_index < _messages.size() - 1 &&
              _messages[_message_index]->done()) {
            log(*_messages[_message_index]);
            ++_message_index;
        }
    }
    void flush_all()
    {
        std::lock_guard<std::mutex> lock{_mutex};
        for(auto& sink : _severity_sinks) {
            sink->flush();
        }
        _data_sink.flush();
    }

private:
    void log(message_guard& message)
    {
        const string msg{message.clear_message()};
        switch(message.channel()) {
            case log_channel::debug_trace:
                log_severity(message.severity(), msg);
                break;
            case log_channel::output_data:
                _data_sink.log(msg);
                break;
            default:
                ORK_UNREACHABLE
        };
    }
    void log_severity(const severity_level severity, const string& message)
    {
        // const bool do_it = ORK_DEBUG || severity > severity_level::debug;
        // if ORK_CONSTEXPR(do_it || true) {
        _severity_sinks[static_cast<size_t>(severity)]->log(message);
        //}
    }
};


struct log_scope::impl {
public:
    std::shared_ptr<message_guard> guard;
    std::shared_ptr<log_multiplexer> multiplexer;
    o_string_stream stream;

public:
    impl(const std::shared_ptr<message_guard>& mg, const std::shared_ptr<log_multiplexer>& mp)
        : guard{mg}
        , multiplexer{mp}
        , stream{}
    {}
    ~impl()
    {
        guard->log(stream);
        multiplexer->on_scope_exit();
    }
    ORK_MOVE_ONLY(impl);
};

log_scope::log_scope(std::unique_ptr<impl>&& ptr)
    : _pimpl{std::move(ptr)}
{}
log_scope::~log_scope() {}

#define LOG_STREAM_OPERATOR(TYPE) \
    log_scope& log_scope::operator<<(const TYPE val) \
    { \
        _pimpl->stream << val; \
        return *this; \
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
    log_scope& log_scope::operator<<(const TYPE val) \
    { \
        _pimpl->stream << ORK_BYTE_2_STR(val); \
        return *this; \
    }

LOG_STREAM_OPERATOR(char)
LOG_STREAM_OPERATOR(char*)
LOG_STREAM_OPERATOR(bstring&)

#undef LOG_STREAM_OPERATOR
#define LOG_STREAM_OPERATOR(TYPE) \
    log_scope& log_scope::operator<<(const TYPE val) \
    { \
        _pimpl->stream << ORK_WIDE_2_STR(val); \
        return *this; \
    }

LOG_STREAM_OPERATOR(wchar_t)
LOG_STREAM_OPERATOR(wchar_t*)
LOG_STREAM_OPERATOR(wstring&)

log_scope& log_scope::operator<<(const void* val)
{
    _pimpl->stream << val;
    return *this;
}
log_scope& log_scope::operator<<(const std::streambuf* sb)
{
    _pimpl->stream << sb;
    return *this;
}
log_scope& log_scope::operator<<(std::ostream& (*pf)(std::ostream&))
{
    _pimpl->stream << pf;
    return *this;
}
log_scope& log_scope::operator<<(std::ios& (*pf)(std::ios&))
{
    _pimpl->stream << pf;
    return *this;
}
log_scope& log_scope::operator<<(std::ios_base& (*pf)(std::ios_base&))
{
    _pimpl->stream << pf;
    return *this;
}


const string& to_formatted_string(const severity_level sv)
{
    static const std::array<const string, severity_levels.size()> strings = {
        {ORK("TRACE"), ORK("DEBUG"), ORK("INFO "), ORK("WARN "), ORK("ERROR"), ORK("FATAL")}};
    return strings[static_cast<size_t>(sv)];
}


struct logger::impl {
public:
    file::path root_directory;
    std::shared_ptr<log_multiplexer> multiplexer;

public:
    impl(const file::path& directory)
        : root_directory(directory)
        , multiplexer{new log_multiplexer(directory)}
    {}
    void flush_all()
    {
        multiplexer->flush_all();
    }
};


logger::logger(const file::path& log_directory)
    : _pimpl(new impl(log_directory))
{}
logger::~logger() {}

const file::path& logger::root_directory()
{
    return _pimpl->root_directory;
}

log_scope logger::get_log_scope(
    const string& file_,
    const string& line_,
    const string& function_,
    const log_channel channel,
    const severity_level severity)
{
    const file::path fullpath(file_);
    string file(fullpath.filename().ORK_GEN_STR());
    file.resize(24, ORK(' '));

    string line(line_);
    line.resize(4, ORK(' '));

    string function(function_);
    function.resize(40, ORK(' '));

    guard_ptr guard{_pimpl->multiplexer->get_message_guard(channel, severity)};
    std::unique_ptr<log_scope::impl> ls_impl(
        new log_scope::impl(guard, _pimpl->multiplexer));
    log_scope scope(std::move(ls_impl));
    // Output formatted context first
    scope << ORK("[") << to_formatted_string(severity) << ORK("] : ") << file
          << ORK("(") << line << ORK(") : ") << function << ORK("-- ");

    // Finally, return the stream to the client
    return std::move(scope);
}

void logger::flush_all()
{
    _pimpl->flush_all();
}


logger* _g_log = nullptr;
int make_global_log(const string& directory)
{
    static logger log(directory);
    _g_log = &log;
    return 0;
}
logger& get_global_log()
{
    return *_g_log;
}


} // namespace ork
