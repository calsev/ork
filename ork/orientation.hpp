/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"ork/ork.hpp"

namespace ork {

enum class orientation {
	pos_x,
	neg_x,
	pos_y,
	neg_y,
	pos_z,
	neg_z
};
orientation operator-(orientation);
//Some executables were using p/n notation and others +/-
//Standardized here
o_stream&operator<<(o_stream&strm, orientation);
i_stream&operator>>(i_stream&strm, orientation&);

const letr*orientation2string(orientation);
orientation string2orientation(const string&val);

std::vector<string>parse_orientation_strings(const string&line);
std::vector<orientation>parse_orientations(const string&);

//The return value is along the positive axis that is collinear with the given axis
double to_wcs(const double val, const orientation o);

//Returns the positive axis that is collinear with the given axis
orientation to_positive(const orientation o);
bool is_positive(const orientation o);

}//namespace ork
