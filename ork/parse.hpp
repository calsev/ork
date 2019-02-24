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

// We give up reusability to reduce compile times
using iterator_type = string::const_iterator;
using context_type = s3::phrase_parse_context<s3::ascii::space_type>::type;

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

} // namespace x3
} // namespace ork


/*
Placeholders for parser components
*/
namespace ork {
namespace orq { // ork-qi :)

BOOST_SPIRIT_TERMINAL(lb_com); //'pound comment'
BOOST_SPIRIT_TERMINAL(lb_com_skip); //'comment skipper'

} // namespace orq
} // namespace ork


/*
Enablers for parser components
*/
namespace boost {
namespace spirit {

// Make custom parser usable as a terminal only, and only for parser expressions (qi::domain).
template<>
struct use_terminal<qi::domain, ork::orq::tag::lb_com> : mpl::true_ {};
template<>
struct use_terminal<qi::domain, ork::orq::tag::lb_com_skip> : mpl::true_ {};

} // namespace spirit
} // namespace boost


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


template<typename char_t, typename iter>
ORK_INLINE bool
consume_lit(char_t const* str, iter& it, const iter& ORK_UNUSED(first), iter const& last)
{
    for(char_t ch = *str; ch != 0; ch = *++str, ++it) {
        if(it == last || (ch != *it)) {
            return false;
        }
    }
    return true;
}

template<typename string_t, typename iter>
ORK_INLINE bool
consume_lit(string_t const& str, iter& it, const iter& ORK_UNUSED(first), iter const& last)
{
    typename string_t::const_iterator str_it = str.begin();
    const typename string_t::const_iterator str_end = str.end();

    for(/**/; str_it != str_end; ++str_it, ++it) {
        if(it == last || (*str_it != *it)) {
            return false;
        }
    }
    return true;
}


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


template<typename iter>
ORK_INLINE bool consume_space(iter& it, const iter& first, const iter& last)
{
    if(it == last) {
        return false;
    }

    while(charset::isspace(*it) && ++it != last) {
    }

    return it != first;
}


template<typename iter>
ORK_INLINE bool consume_lb_com(iter& it, const iter& ORK_UNUSED(first), const iter& last)
{
    if(it == last) {
        return false;
    }

    if(*it++ != ORK('#')) { // Consume the marker
        return false;
    }
    while(it != last && *it != ORK('\n')) { // Up to but do not consume the eol
        ++it;
    }

    return true; // We consumed at least the first character
}


} // namespace detail


struct ORK_ORK_API lb_com_parser : qi::primitive_parser<lb_com_parser> {
public: // Parser component stuff
    template<typename context, typename iter>
    struct attribute { // Define the attribute type exposed by this parser component
        typedef qi::unused_type type;
    };

    // This function is called during the actual parsing process
    template<typename iter, typename context, typename skipper, typename attribute>
    bool parse(
        iter& first,
        const iter& last,
        context& ORK_UNUSED(ctxt),
        const skipper& skip,
        attribute& ORK_UNUSED(attr)) const
    {
        boost::spirit::qi::skip_over(first, last, skip); // All primitive parsers pre-skip

        iter it(first);
        if(!detail::consume_lb_com(it, first, last)) {
            return false;
        }

        first = it;
        return true;
    }

    // This function is called during error handling to create a human readable string for the error context.
    template<typename context>
    boost::spirit::info what(context&) const
    {
        return boost::spirit::info(BORK("lb_com"));
    }
};


struct ORK_ORK_API lb_com_skip_parser : qi::primitive_parser<lb_com_skip_parser> {
public: // Parser component stuff
    template<typename context, typename iter>
    struct attribute { // Define the attribute type exposed by this parser component
        typedef qi::unused_type type;
    };

    // This function is called during the actual parsing process
    template<typename iter, typename context, typename skipper, typename attribute>
    bool parse(
        iter& first,
        const iter& last,
        context& ORK_UNUSED(ctxt),
        const skipper& ORK_UNUSED(skip),
        attribute& ORK_UNUSED(attr)) const
    {
        const iter first_copy(first);
        do {
            iter it(first);
            if(detail::consume_space(it, first, last)) {
                first = it;
                continue;
            }

            it = first;
            if(detail::consume_lb_com(it, first, last)) {
                first = it;
                continue;
            }

            break;
        } while(true);
        return first != first_copy;
    }

    // This function is called during error handling to create a human readable string for the error context.
    template<typename context>
    boost::spirit::info what(context&) const
    {
        return boost::spirit::info(BORK("lb_com_skip"));
    }
};


} // namespace orq
} // namespace ork


/*
Instantiators for parser components
*/
namespace boost {
namespace spirit {
namespace qi {

// This is the factory function object that is invoked to create an instance of our parser.
#    define ORK_ORQ_FACTORY(TAG) \
        template<typename modifiers> \
        struct make_primitive<ork::orq::tag::TAG, modifiers> { \
            typedef typename ork::orq::ORK_CAT(TAG, _parser) result_type; \
            result_type operator()(unused_type, unused_type) const \
            { \
                return result_type(); \
            } \
        };

ORK_ORQ_FACTORY(lb_com);
ORK_ORQ_FACTORY(lb_com_skip);

} // namespace qi
} // namespace spirit
} // namespace boost


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
