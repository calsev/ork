/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"

#if ORK_USE_BOOST

#    define ORK_BOOST_INC_FILE "boost/spirit/home/x3.hpp"
#    include "ork/core/boost_include.inl"

namespace ork {
namespace x3 {

namespace s3 = boost::spirit::x3;

#    define ORK_PARSER_DECL(ID, ATTR_TYPE) \
        using ORK_CAT(ID, _type) = ork::x3::s3::rule<class ID, ATTR_TYPE>; \
        BOOST_SPIRIT_DECLARE(ORK_CAT(ID, _type)) \
        ORK_CAT(ID, _type) ID()

#    define ORK_PARSER_DEF(ID) \
        ORK_CAT(ID, _type) const ORK_CAT(ID, _instance) = ORK_STR(ID); \
        BOOST_SPIRIT_DEFINE(ID); \
        BOOST_SPIRIT_INSTANTIATE(ORK_CAT(ID, _type), iterator_type, context_type) \
        ORK_CAT(ID, _type) ID() \
        { \
            return ORK_CAT(ID, _instance); \
        }

// Bool, no 1/0 allowed
ORK_PARSER_DECL(alpha_bool, bool);

// An identifier is a typical C/C++/Java/C# name: start with underscore/letter, continue with underscore/letter/digit
ORK_PARSER_DECL(id, string);

// A 'name' is a bit of a muddy combination of proper name(e.g.Harold, Hawai'i, Dan-Man) and a user name (e.g. cool_guy_32, _hack0r_0110)
// We require : start with letter / underscore, continue with letter / digit / underscore / dash / apostrophe
ORK_PARSER_DECL(name, string);

// A quoted string - does not support escaped quotes
ORK_PARSER_DECL(quote, string);

// An @variable is more typical of single-pass interpreters that require a variable tag (e.g. $, @) but may allow looser rules for the name
// We require: start with letter / digit / underscore, continue with the same
ORK_PARSER_DECL(at_var, string);

// comment skipper: skips whitespace, #-comments and //-comments
ORK_PARSER_DECL(com_skip, s3::unused_type);

// We give up reusability to reduce compile times
using iterator_type = string::const_iterator;
using context_type = s3::phrase_parse_context<com_skip_type>::type;


template<typename T>
struct numeric;
#    define ORK_X3_NUMERIC(TYPE, PARSER) \
        template<> \
        struct numeric<TYPE> { \
            static ORK_CONSTEXPR auto tag() -> decltype(PARSER)& \
            { \
                return PARSER; \
            } \
        };

// So we can use key-value template, etc.
ORK_X3_NUMERIC(bool, alpha_bool);

ORK_X3_NUMERIC(unsigned short, s3::ushort_);
ORK_X3_NUMERIC(unsigned, s3::uint_);
ORK_X3_NUMERIC(unsigned long, s3::ulong_);
ORK_X3_NUMERIC(unsigned long long, s3::ulong_long);

ORK_X3_NUMERIC(short, s3::short_);
ORK_X3_NUMERIC(int, s3::int_);
ORK_X3_NUMERIC(long, s3::long_);
ORK_X3_NUMERIC(long long, s3::long_long);

ORK_X3_NUMERIC(float, s3::float_);
ORK_X3_NUMERIC(double, s3::double_);
// ORK_X3_NUMERIC(long double, s3::long_double);

} // namespace x3
} // namespace ork

#endif // ORK_USE_BOOST
