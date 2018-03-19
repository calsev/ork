/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#    error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_LOG_HPP
#    error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_LOG_HPP
#    define ORK_LOG_HPP


namespace ork {


enum class log_channel {
    debug_trace // For now all error, debug, trace goes to one file
    ,
    output_data // For now, all program output goes to one file
};
const std::array<log_channel, 2> log_channels = {log_channel::debug_trace,
                                                 log_channel::output_data};
const ork::string& to_string(const log_channel);
log_channel string2log_channel(const ork::string&);


enum class severity_level { trace, debug, info, warning, error, fatal };
const std::array<severity_level, 6> severity_levels = {severity_level::trace,
                                                       severity_level::debug,
                                                       severity_level::info,
                                                       severity_level::warning,
                                                       severity_level::error,
                                                       severity_level::fatal};
const ork::string& to_string(const severity_level);
severity_level string2severity_level(const ork::string&);


class log_scope {
public:
    struct impl;

private:
    std::unique_ptr<impl> _pimpl;

public:
    ORK_ORK_API log_scope(std::unique_ptr<impl>&&);
    ORK_ORK_API ~log_scope();
    ORK_MOVE_ONLY(log_scope);

public:
    ORK_ORK_API log_scope& operator<<(const bool val);
    ORK_ORK_API log_scope& operator<<(const short val);
    ORK_ORK_API log_scope& operator<<(const unsigned short val);
    ORK_ORK_API log_scope& operator<<(const int val);
    ORK_ORK_API log_scope& operator<<(const unsigned int val);
    ORK_ORK_API log_scope& operator<<(const long val);
    ORK_ORK_API log_scope& operator<<(const unsigned long val);
    ORK_ORK_API log_scope& operator<<(const long long val);
    ORK_ORK_API log_scope& operator<<(const unsigned long long val);
    ORK_ORK_API log_scope& operator<<(const float val);
    ORK_ORK_API log_scope& operator<<(const double val);
    ORK_ORK_API log_scope& operator<<(const long double val);

    ORK_ORK_API log_scope& operator<<(const char);
    ORK_ORK_API log_scope& operator<<(const char*);
    ORK_ORK_API log_scope& operator<<(const bstring&);

    ORK_ORK_API log_scope& operator<<(const wchar_t);
    ORK_ORK_API log_scope& operator<<(const wchar_t*);
    ORK_ORK_API log_scope& operator<<(const wstring&);

    ORK_ORK_API log_scope& operator<<(const void* val);

    ORK_ORK_API log_scope& operator<<(const std::streambuf* sb);

    ORK_ORK_API log_scope& operator<<(std::ostream& (*pf)(std::ostream&));
    ORK_ORK_API log_scope& operator<<(std::ios& (*pf)(std::ios&));
    ORK_ORK_API log_scope& operator<<(std::ios_base& (*pf)(std::ios_base&));
};


class logger {
private:
    struct impl;

private:
    std::unique_ptr<impl> _pimpl;

public:
    ORK_ORK_API explicit logger(const file::path& log_file);
    ORK_ORK_API ~logger();
    ORK_NON_COPYABLE(logger);

public:
    ORK_ORK_API const file::path& root_directory();
    ORK_ORK_API log_scope get_log_scope(
        const string& file,
        const string& line,
        const string& function,
        const log_channel channel,
        const severity_level severity);
    ORK_ORK_API void flush_all();
};


ORK_ORK_EXT(int) make_global_log(const string& directory);
ORK_ORK_EXT(logger&) get_global_log();
#    define ORK_GLOBAL_LOG(LOG_DIRECTORY) \
        const int g_ork_log_val = ork::make_global_log(LOG_DIRECTORY);


#    define _ORK_LOG(CH, SV, UID) \
        auto UID{ork::get_global_log().get_log_scope( \
            ORK_FILEN, ORK_LINE, ORK_FUNC, CH, SV)}; \
        UID
#    define ORK_LOG(SV) \
        _ORK_LOG(ork::log_channel::debug_trace, SV, ORK_UID(_ork_log_log_))
#    define ORK_LOUT \
        _ORK_LOG(ork::log_channel::output_data, ork::severity_level::info, ORK_UID(_ork_log_out_))

#    define ORK_LOC_BLOCK \
        ORK("\n -- ") << ORK_FLOC_STREAM << ORK(":\n -- ") << ORK_FUNC << ORK("\n")


} // namespace ork

#endif // ORK_LOG_HPP
