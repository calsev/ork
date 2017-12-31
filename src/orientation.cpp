/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<string>
#include<sstream>

#include"ork/glm.hpp"
#include"ork/orientation.hpp"
#include"ork/string_utils.hpp"


namespace ork {


orientation operator-(orientation or ) {
	switch(or ) {
	case orientation::pos_x:
		return orientation::neg_x;
	case orientation::neg_x:
		return orientation::pos_x;
	case orientation::pos_y:
		return orientation::neg_y;
	case orientation::neg_y:
		return orientation::pos_y;
	case orientation::pos_z:
		return orientation::neg_z;
	case orientation::neg_z:
		return orientation::pos_z;
	}
	ORK_THROW(ORK("Unreachable"));
}

//These must be in the same order as the enum
static const char_t* oriention_strings[] = {
	ORK("+X"),
	ORK("-X"),
	ORK("+Y"),
	ORK("-Y"),
	ORK("+Z"),
	ORK("-Z")
};


o_stream&operator<<(o_stream&strm, orientation or ) {
	strm << orientation2string(or );
	return strm;
}


i_stream&operator >> (i_stream&strm, orientation& or ) {
	string val;
	strm >> val;
	or = string2orientation(val);
	return strm;
}


const char_t*orientation2string(orientation or ) {
	const size_t index = static_cast<size_t>(or );
	if(index < sizeof(oriention_strings) / sizeof(*oriention_strings)) return oriention_strings[index];
	else ORK_THROW(ORK("Attempt to convert invalid orientation"));
}


orientation string2orientation(const string&val) {
	string upper = val;
	to_upper(upper);

	const size_t num_strings = sizeof(oriention_strings) / sizeof(*oriention_strings);
	LOOPI(num_strings) {
		if(upper == oriention_strings[i]) {
			return static_cast<orientation>(i);
		}
	}
	ORK_THROW(ORK("Attempt to convert invalid orientation: ") << val);
}


//Crappy little parser to get boost out of this compile unit
using iter_t = string::const_iterator;
void skip_whitespace(iter_t&it, const iter_t&end) {
	while(it != end && (*it == ' ' || *it == ' ')) {//Whitespace can be space or tab
		++it;
	}
}
iter_t consume_token(iter_t begin, const iter_t&end) {
	while(begin != end && *begin != ' ' && *begin != ' ') {//Whitespace can be space or tab
		++begin;
	}
	return begin;
}
std::vector<string>parse_orientation_strings(const string&str) {
	std::vector<string>retval;
	iter_t curr_pos{str.begin()};
	const iter_t end{str.end()};
	while(curr_pos != end) {
		skip_whitespace(curr_pos, end);
		const iter_t new_pos = consume_token(curr_pos, end);
		if(new_pos != curr_pos) {
			retval.emplace_back(curr_pos, new_pos);
		}
		curr_pos = new_pos;
	}
	return std::move(retval);
}


std::vector<orientation>parse_orientations(const string&line) {
	const std::vector<string>strings = parse_orientation_strings(line);
	std::vector<orientation>retval;
	for(const string&orient : strings) {
		retval.push_back(string2orientation(orient));
	}
	return std::move(retval);
}


double to_wcs(const double val, const orientation o) {
	switch(o) {
	case orientation::pos_x:
	case orientation::pos_y:
	case orientation::pos_z:
		return val;
	case orientation::neg_x:
	case orientation::neg_y:
	case orientation::neg_z:
		return -val;
	}
	ORK_THROW(ORK("Unreachable"));
}


orientation to_positive(const orientation o) {
	switch(o) {
	case orientation::pos_x:
		return orientation::pos_x;
	case orientation::neg_x:
		return orientation::pos_x;
	case orientation::pos_y:
		return orientation::pos_y;
	case orientation::neg_y:
		return orientation::pos_y;
	case orientation::pos_z:
		return orientation::pos_z;
	case orientation::neg_z:
		return orientation::pos_z;
	}
	ORK_THROW(ORK("Unreachable"));
}

bool is_positive(const orientation o) {
	switch(o) {
	case orientation::pos_x:
	case orientation::pos_y:
	case orientation::pos_z:
		return true;
	case orientation::neg_x:
	case orientation::neg_y:
	case orientation::neg_z:
		return false;
	}
	ORK_THROW(ORK("Unreachable"));
}


}//namespace ork
