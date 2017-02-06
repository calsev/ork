/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"boost/spirit/home/support/common_terminals.hpp"
#include"boost/spirit/home/qi.hpp"
#include"ork/ork.hpp"


/*
Placeholders for parser components
*/
namespace ork {
namespace orq {//ork-qi :)

BOOST_SPIRIT_TERMINAL(id);
BOOST_SPIRIT_TERMINAL(quote);
BOOST_SPIRIT_TERMINAL(lb_com);//'pound comment'
BOOST_SPIRIT_TERMINAL(lb_com_skip);//'comment skipper'

}//namespace orq
}//namespace ork


/*
Enablers for parser components
*/
namespace boost {
namespace spirit {

//Make custom parser usable as a terminal only, and only for parser expressions (qi::domain).
template<> struct use_terminal<qi::domain, ork::orq::tag::id> : mpl::true_ {};
template<> struct use_terminal<qi::domain, ork::orq::tag::quote> : mpl::true_ {};
template<> struct use_terminal<qi::domain, ork::orq::tag::lb_com> : mpl::true_ {};
template<> struct use_terminal<qi::domain, ork::orq::tag::lb_com_skip> : mpl::true_ {};

}//namespace spirit
}//namespace boost



namespace ork {
namespace orq {//ork-qi :)


namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace proto = boost::proto;


#if ORK_UNICODE
using charset = spirit::char_encoding::standard_wide;
using space_type = spirit::standard_wide::space_type;
using spirit::standard_wide::space;
#else
using charset = spirit::char_encoding::standard;
using space_type = spirit::standard::space_type;
using spirit::standard::space;
#endif


namespace detail {


template<typename char_t, typename iter>
ORK_INLINE bool consume_lit(char_t const* str, iter&it, const iter& first, iter const& last) {
	for(char_t ch = *str; ch != 0; ch = *++str, ++it) {
		if(it == last || (ch != *it)) {
			return false;
		}
	}
	return true;
}

template <typename string_t, typename iter>
ORK_INLINE bool consume_lit(string_t const& str, iter&it, const iter& first, iter const& last) {
	typename string_t::const_iterator str_it = str.begin();
	const typename string_t::const_iterator str_end = str.end();

	for(/**/; str_it != str_end; ++str_it, ++it) {
		if(it == last || (*str_it != *it)) {
			return false;
		}
	}
	return true;
}


template<typename iter>
ORK_INLINE bool consume_identifier(iter& it, const iter&first, const iter& last) {
	if(it == last) {
		return false;
	}

	auto ch = *it;
	if(!std::isalpha(ch) && ch != ORK('_')) {//NOT charset: this is programming language (ascii) identifier
		return false;//First character must be letter or underscore
	}
	while((std::isalnum(ch) || ch == ORK('_')) && ++it != last) {//NOT charset: this is programming language (ascii) identifier
		ch = *it;//Subsequent characters can be numbers also
	}

	return true;//We consumed at least the first character
}


template<typename iter>
ORK_INLINE bool consume_quote(iter&it, const iter& first, const iter& last) {
	if(it == last) {
		return false;
	}

	if(*it++ != ORK('"')) {//Consume the quote
		return false;
	}
	for(/**/; *it != ORK('"'); ++it) {//Up to but do not consume the quote
		if(it == last) {
			return false;
		}
	}

	++it;//Consume second in pair of quotes
	return true;//Allow empty attribute
}


template<typename iter>
ORK_INLINE bool consume_space(iter&it, const iter& first, const iter& last) {
	if(it == last) {
		return false;
	}

	while(charset::isspace(*it) && ++it != last) {}

	return it != first;
}


template<typename iter>
ORK_INLINE bool consume_lb_com(iter&it, const iter& first, const iter& last) {
	if(it == last) {
		return false;
	}

	if(*it++ != ORK('#')) {//Consume the marker
		return false;
	}
	while(it != last && *it != ORK('\n')) {//Up to but do not consume the eol
		++it;
	}

	return true;//We consumed at least the first character
}


}//namespace detail


struct ORK_ORK_API id_parser : qi::primitive_parser<id_parser> {
public://Parser component stuff
	template<typename context, typename iter>
	struct attribute {//Define the attribute type exposed by this parser component
		typedef string type;
	};

	//This function is called during the actual parsing process
	template<typename iter, typename context, typename skipper, typename attribute>
	bool parse(iter& first, const iter& last, context&ctxt, const skipper& skip, attribute& attr) const {
		boost::spirit::qi::skip_over(first, last, skip);//All primitive parsers pre-skip

		iter it(first);
		if(!detail::consume_identifier(it, first, last)) {
			return false;
		}

		attribute result(first, it);
		first = it;
		spirit::traits::assign_to(result, attr);
		return true;
	}

	//This function is called during error handling to create a human readable string for the error context.
	template<typename context>
	boost::spirit::info what(context&) const {
		return boost::spirit::info("id");
	}
};


struct ORK_ORK_API quote_parser : qi::primitive_parser<quote_parser> {
public://Parser component stuff
	template<typename context, typename iter>
	struct attribute {//Define the attribute type exposed by this parser component
		typedef string type;
	};

	//This function is called during the actual parsing process
	template<typename iter, typename context, typename skipper, typename attribute>
	bool parse(iter& first, const iter& last, context&ctxt, const skipper& skip, attribute& attr) const {
		boost::spirit::qi::skip_over(first, last, skip);//All primitive parsers pre-skip

		iter it(first);
		if(!detail::consume_quote(it, first, last)) {
			return false;
		}

		attribute result(++first, it - 1);//Do not include quotes; preserve it
		first = it;
		spirit::traits::assign_to(result, attr);
		return true;
	}

	//This function is called during error handling to create a human readable string for the error context.
	template<typename context>
	boost::spirit::info what(context&) const {
		return boost::spirit::info("quote");
	}
};


struct ORK_ORK_API lb_com_parser : qi::primitive_parser<lb_com_parser> {
public://Parser component stuff
	template<typename context, typename iter>
	struct attribute {//Define the attribute type exposed by this parser component
		typedef qi::unused_type type;
	};

	//This function is called during the actual parsing process
	template<typename iter, typename context, typename skipper, typename attribute>
	bool parse(iter& first, const iter& last, context&ctxt, const skipper& skip, attribute& attr) const {
		boost::spirit::qi::skip_over(first, last, skip);//All primitive parsers pre-skip

		iter it(first);
		if(!detail::consume_lb_com(it, first, last)) {
			return false;
		}

		first = it;
		return true;
	}

	//This function is called during error handling to create a human readable string for the error context.
	template<typename context>
	boost::spirit::info what(context&) const {
		return boost::spirit::info("lb_com");
	}
};


struct ORK_ORK_API lb_com_skip_parser : qi::primitive_parser<lb_com_skip_parser> {
public://Parser component stuff
	template<typename context, typename iter>
	struct attribute {//Define the attribute type exposed by this parser component
		typedef qi::unused_type type;
	};

	//This function is called during the actual parsing process
	template<typename iter, typename context, typename skipper, typename attribute>
	bool parse(iter& first, const iter& last, context&ctxt, const skipper& skip, attribute& attr) const {
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

	//This function is called during error handling to create a human readable string for the error context.
	template<typename context>
	boost::spirit::info what(context&) const {
		return boost::spirit::info("lb_com_skip");
	}
};


}//namespace orq
}//namespace ork


/*
Instantiators for parser components
*/
namespace boost {
namespace spirit {
namespace qi {

//This is the factory function object that is invoked to create an instance of our parser.
#define ORK_ORQ_FACTORY(TAG)\
template<typename modifiers>\
struct make_primitive<ork::orq::tag::TAG, modifiers> {\
	typedef typename ork::orq::ORK_CAT(TAG,_parser) result_type;\
	result_type operator()(unused_type, unused_type) const {\
		return result_type();\
	}\
};

ORK_ORQ_FACTORY(id);
ORK_ORQ_FACTORY(quote);
ORK_ORQ_FACTORY(lb_com);
ORK_ORQ_FACTORY(lb_com_skip);

}//namespace qi
}//namespace spirit
}//namespace boost


namespace ork {
namespace orq {


template<typename T>struct numeric;
#define ORK_ORQ_NUMERIC(TYPE, TAG) \
template<>struct numeric<TYPE> {\
	static ORK_CONSTEXPR auto tag()->decltype(TAG)& {\
		return TAG;\
	}\
};

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


}//namespace orq
}//namespace ork
