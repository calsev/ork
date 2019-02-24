/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"

#if ORK_USE_BOOST

#    define ORK_BOOST_INC_FILE "boost/spirit/home/x3.hpp"
#    include "ork/core/boost_include.inl"
#    define ORK_BOOST_INC_FILE "boost/spirit/home/support/common_terminals.hpp"
#    include "ork/core/boost_include.inl"
#    define ORK_BOOST_INC_FILE "boost/spirit/home/qi.hpp"
#    include "ork/core/boost_include.inl"

namespace ork {
namespace x3 {

namespace s3 = boost::spirit::x3;

#    define ORK_PARSER_DECL(ID, ATTR_TYPE) \
        using ORK_CAT(ID, _type) = s3::rule<class ID, ATTR_TYPE>; \
        BOOST_SPIRIT_DECLARE(ORK_CAT(ID, _type)) \
        ORK_CAT(ID, _type) ID()

#    define ORK_PARSER_DEF(ID) \
        namespace impl { \
        ORK_CAT(ID, _type) const ID = ORK_STR(ID); \
        BOOST_SPIRIT_DEFINE(ID); \
        BOOST_SPIRIT_INSTANTIATE(ORK_CAT(ID, _type), iterator_type, context_type) \
        } /*namespace impl*/ \
        ORK_CAT(ID, _type) ID() \
        { \
            return impl::ID; \
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

} // namespace x3
} // namespace ork


namespace ork {
namespace orq { // ork-qi :)


namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace proto = boost::proto;


#    if ORK_UNICODE
using charset = spirit::char_encoding::standard_wide;
using space_type = spirit::standard_wide::space_type;
using spirit::standard_wide::space;
#    else
using charset = spirit::char_encoding::standard;
using space_type = spirit::standard::space_type;
using spirit::standard::space;
#    endif


namespace detail {


/*
A decimal integer is one or more digits
*/
template<typename iter>
ORK_INLINE bool consume_dec_int(iter& it, const iter& first, const iter& last)
{
    if(it == last) {
        return false;
    }

    if(*it == ORK('-') && ++it != last) {
        /*Do Nothing*/
    }
    while(std::isdigit(*it) &&
          ++it != last) { // NOT charset: this is programming language (ascii) identifier
        /*Do Nothing*/
    }

    return it != first;
}


/*
An real is one or more digits, maybe a point, maybe more digits
We therefore allow C-style terminal decimal point
*/
template<typename iter>
ORK_INLINE bool consume_real(iter& it, const iter& first, const iter& last)
{
    if(it == last) {
        return false;
    }

    if(*it == ORK('-') && ++it != last) {
        /*Do Nothing*/
    }
    while((std::isdigit(*it)) &&
          ++it != last) { // NOT charset: this is programming language (ascii) identifier
        /*Do Nothing*/
    }
    if(it == first) {
        return false; // We must consume at least one digit
    }
    if(*it == ORK('.') && ++it != last) {
        /*Do Nothing*/
    }
    while((std::isdigit(*it)) &&
          ++it != last) { // NOT charset: this is programming language (ascii) identifier
        /*Do Nothing*/
    }

    return true; // We consumed at least one digit
}


} // namespace detail


} // namespace orq
} // namespace ork


namespace ork {
namespace orq {


template<typename T>
struct numeric;
#    define ORK_ORQ_NUMERIC(TYPE, TAG) \
        template<> \
        struct numeric<TYPE> { \
            static ORK_CONSTEXPR auto tag() -> decltype(TAG)& \
            { \
                return TAG; \
            } \
        };

ORK_ORQ_NUMERIC(bool, alpha_bool); // So we can use key-value template,etc.

ORK_ORQ_NUMERIC(unsigned short, qi::ushort_);
ORK_ORQ_NUMERIC(unsigned, qi::uint_);
ORK_ORQ_NUMERIC(unsigned long, qi::ulong_);
ORK_ORQ_NUMERIC(unsigned long long, qi::ulong_long);

ORK_ORQ_NUMERIC(short, qi::short_);
ORK_ORQ_NUMERIC(int, qi::int_);
ORK_ORQ_NUMERIC(long, qi::long_);
ORK_ORQ_NUMERIC(long long, qi::long_long);

ORK_ORQ_NUMERIC(float, qi::float_);
ORK_ORQ_NUMERIC(double, qi::double_);
ORK_ORQ_NUMERIC(long double, qi::long_double);


} // namespace orq
} // namespace ork

#endif // ORK_USE_BOOST
