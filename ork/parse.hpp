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
namespace orq {

BOOST_SPIRIT_TERMINAL(id);

}//namespace ork
}//namespace ork


/*
Enablers for parser components
*/
namespace boost {
namespace spirit {

//Make custom_parser::iter_pos usable as a terminal only, and only for parser expressions (qi::domain).
template <>
struct use_terminal<qi::domain, ork::orq::tag::id> : mpl::true_ {};

}//namespace spirit
}//namespace boost



namespace ork {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace proto = boost::proto;


#if UNICODE
typedef spirit::char_encoding::standard_wide charset;
#else
typedef spirit::char_encoding::standard charset;
#endif


namespace orq {//ork-qi :)


struct id_parser : qi::primitive_parser<id_parser> {
public://Parser component stuff
	template<typename context, typename iter>
	struct attribute {//Define the attribute type exposed by this parser component
		typedef string type;
	};

	//This function is called during the actual parsing process
	template<typename iter, typename context, typename skipper, typename attribute>
	bool parse(iter& first, const iter& last, context&ctxt, const skipper& skip, attribute& attr) const {
		boost::spirit::qi::skip_over(first, last, skip);//All primitive parsers pre-skip
		//TODO//The actual parsing
		return true;
	}

	//This function is called during error handling to create a human readable string for the error context.
	template<typename context>
	boost::spirit::info what(context&) const {
		return boost::spirit::info("id");
	}
};


struct identifier : qi::grammar<string::const_iterator, string(), ascii::space_type> {
public:
	typedef string::const_iterator iter;
public:
	identifier() : identifier::base_type(start) {
		first %=
			qi::alpha | qi::char_(ORK('_'))
			;
		rest %=
			qi::alnum | qi::char_(ORK('_'))
			;
		start %=
			qi::lexeme[first >> *rest]
			;
	}
public:
	qi::rule<iter, letr()> first;
	qi::rule<iter, letr()> rest;
	qi::rule<iter, string(), ascii::space_type> start;
};


}//namespace orq
}//namespace ork
