/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<string>
#include<sstream>
#include"boost/algorithm/string/case_conv.hpp"
#include"boost/tokenizer.hpp"

#include"ork/glm.hpp"
#include"ork/orientation.hpp"


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
static const letr* oriention_strings[] = {
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


const letr*orientation2string(orientation or ) {
	const size_t index = static_cast<size_t>(or );
	if(index < sizeof(oriention_strings) / sizeof(*oriention_strings)) return oriention_strings[index];
	else ORK_THROW(ORK("Attempt to convert invalid orientation"));
}


orientation string2orientation(const string&val) {
	string upper = val;
	boost::algorithm::to_upper(upper);

	const size_t num_strings = sizeof(oriention_strings) / sizeof(*oriention_strings);
	LOOPI(num_strings) {
		if(upper == oriention_strings[i]) {
			return static_cast<orientation>(i);
		}
	}
	ORK_THROW(ORK("Attempt to convert invalid orientation: ") << val);
}


std::vector<string>parse_orientation_strings(const string&line) {
	std::vector<string>retval;
	//Build the orientations
	typedef boost::tokenizer<boost::char_separator<letr> > tokenizer;
	boost::char_separator<letr> delim(ORK(" \t"));//Whitespace can be space or tab
	tokenizer tokens(line, delim);
	for(const string&str : tokens) {
		retval.push_back(str);
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
