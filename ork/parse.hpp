/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"boost/spirit/home/support/common_terminals.hpp"
#include"ork/ork.hpp"


namespace ork {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace tag = spirit::tag;
namespace proto = boost::proto;


#if UNICODE
typedef spirit::char_encoding::standard_wide charset;
#else
typedef spirit::char_encoding::standard charset;
#endif

namespace parse {


struct identifier : qi::grammar<string::const_iterator, string, ascii::space_type> {
public:
	typedef std::string::const_iterator iter;
public:
	identifier() : identifier::base_type(start) {
		start %= qi::lexeme[(qi::alpha | qi::char_(TXT('_'))) >> *(qi::alnum | qi::char_(TXT('_')))]
		;
	}
public:
	qi::rule<string::const_iterator, ork::string, ascii::space_type> start;
};


}//namespace parse
}//namespace ork
