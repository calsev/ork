/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include<vector>
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
ORK_ORK_EXT(orientation) operator-(orientation);
//Some executables were using p/n notation and others +/-
//Standardized here
ORK_ORK_EXT(o_stream&) operator<<(o_stream&strm, const orientation);
ORK_ORK_EXT(i_stream&) operator>>(i_stream&strm, orientation&);

ORK_ORK_EXT(const char_t*) orientation2string(const orientation);
ORK_ORK_EXT(orientation) string2orientation(const string&);

ORK_ORK_EXT(std::vector<string>) parse_orientation_strings(const string&line);
ORK_ORK_EXT(std::vector<orientation>) parse_orientations(const string&);

//The return value is along the positive axis that is collinear with the given axis
ORK_ORK_EXT(double) to_wcs(const double val, const orientation o);

//Returns the positive axis that is collinear with the given axis
ORK_ORK_EXT(orientation) to_positive(const orientation o);
ORK_ORK_EXT(bool) is_positive(const orientation o);

}//namespace ork
