/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#define _USE_MATH_DEFINES//M_PI
#include<cmath>
#include<iomanip>
#include"ork/geometry.hpp"
#include"ork/glm.hpp"

#include"glm/mat3x3.hpp"
#include"glm/gtx/vector_angle.hpp"


namespace ork {

double mm2inch(double mm) {
	return mm / 25.4;
}

// Given an angle in radians, return that angle in degrees
double rad2deg(double rad) {
	return rad * 180.0 / M_PI;
}

// Given an angle in degrees, return that angle in radians
double deg2rad(double deg) {
	return deg * M_PI / 180.0;
}

//Given an angle in Radians, return the equivalent angle on [0, 2*pi]
double simple_angle(double angle) {
	while(angle < 0.) {
		angle += 2. * M_PI;
	}
	return std::fmod(angle, 2.0 * M_PI);
}


string to_dimension(const double coord) {
	string_stream stream;
	stream << std::fixed << std::showpoint << std::setprecision(4);
	stream << coord;
	return stream.str();
}


rotation_direction operator-(const rotation_direction dir) {
	switch(dir) {
	case rotation_direction::clockwise:
		return rotation_direction::counter_clockwise;
	case rotation_direction::counter_clockwise:
		return rotation_direction::clockwise;
	}
	ORK_THROW(ORK("Unreachable"));
}


const double circle<angle::radian>::full = 2.*M_PI;
const double circle<angle::degree>::full = 360.;


namespace GLM {


namespace MC {

const glm::dmat3x3 zero_mat(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 0.0));
/*
These are transform matrices into MC view LCS.
As always, these are column major.
*/
const glm::dmat3x3 to_top(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0));
const glm::dmat3x3 to_front(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(0.0, -1., 0.0));
const glm::dmat3x3 to_back(glm::dvec3(-1., 0.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(0.0, 1.0, 0.0));
const glm::dmat3x3 to_bottom(glm::dvec3(-1., 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, -1.));
const glm::dmat3x3 to_right(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(1.0, 0.0, 0.0));
const glm::dmat3x3 to_left(glm::dvec3(0.0, -1., 0.0), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(-1., 0.0, 0.0));


const glm::dmat3x3&lcs_mat(const orientation axis) {
	switch(axis) {
	case orientation::pos_x:
		return to_left;
	case orientation::neg_x:
		return to_right;
	case orientation::pos_y:
		return to_front;
	case orientation::neg_y:
		return to_back;
	case orientation::pos_z:
		return to_bottom;
	case orientation::neg_z:
		return to_top;
	}
	ORK_THROW(ORK("Unreachable"));
}


glm::dvec3 to_lcs(const glm::dvec3&vec, const orientation axis) {
	return vec * lcs_mat(axis);
}

glm::dvec3 to_lcs(const double val, const orientation axis) {
	return val * orientation2direction(axis).get() * lcs_mat(axis);
}


const glm::dmat3x3 from_top(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0));
const glm::dmat3x3 from_front(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, -1.), glm::dvec3(0.0, 1.0, 0.0));
const glm::dmat3x3 from_back(glm::dvec3(-1., 0.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(0.0, 1.0, 0.0));
const glm::dmat3x3 from_bottom(glm::dvec3(-1., 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, -1.));
const glm::dmat3x3 from_right(glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0));
const glm::dmat3x3 from_left(glm::dvec3(0.0, 0.0, -1.), glm::dvec3(-1., 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0));


const glm::dmat3x3&wcs_mat(const orientation axis) {
	switch(axis) {
	case orientation::pos_x:
		return from_left;
	case orientation::neg_x:
		return from_right;
	case orientation::pos_y:
		return from_front;
	case orientation::neg_y:
		return from_back;
	case orientation::pos_z:
		return from_bottom;
	case orientation::neg_z:
		return from_top;
	}
	ORK_THROW(ORK("Unreachable"));
}


glm::dvec3 to_wcs(const glm::dvec3&vec, const orientation axis) {
	return vec * wcs_mat(axis);
}


glm::dvec3 to_wcs(const double val, const orientation axis) {
	return val * orientation2direction(axis).get() * wcs_mat(axis);
}


rotated_view rotated(const view&v, const rotation_direction dir, const int rotations) {
	//CC is increasing due to how we defined our members
	int index = dir == rotation_direction::counter_clockwise ? rotations : -rotations;
	//Now wrap around the ends
	while(index < 0)index += 4;
	index %= 4;
	rotated_view retval(dir, rotations);
	LOOPVI(v.dirs) {
		retval.dirs[i] = v.dirs[index];
		++index;
		index %= 4;
	}
	return retval;
}


const view vright(orientation::pos_y, orientation::pos_z, orientation::neg_y, orientation::neg_z);
const view vleft(orientation::neg_y, orientation::pos_z, orientation::pos_y, orientation::neg_z);
const view vfront(orientation::pos_x, orientation::pos_z, orientation::neg_x, orientation::neg_z);
const view vback(orientation::neg_x, orientation::pos_z, orientation::pos_x, orientation::neg_z);
const view vtop(orientation::pos_x, orientation::pos_y, orientation::neg_x, orientation::neg_y);
const view vbottom(orientation::neg_x, orientation::pos_y, orientation::pos_x, orientation::neg_y);

const view&get_view(const orientation axis) {
	switch(axis) {
	case orientation::pos_x:
		return vleft;
	case orientation::neg_x:
		return vright;
	case orientation::pos_y:
		return vfront;
	case orientation::neg_y:
		return vback;
	case orientation::pos_z:
		return vbottom;
	case orientation::neg_z:
		return vtop;
	}
	ORK_THROW(ORK("Unreachable"));
}

}//namespace MC


glm::dvec3 point_on_plane(const glm::dvec3&vec, const orientation axis) {
	switch(axis) {
	case orientation::pos_x:
	case orientation::neg_x:
		return glm::dvec3(0., vec.y, vec.z);
		break;
	case orientation::pos_y:
	case orientation::neg_y:
		return glm::dvec3(vec.x, 0., vec.z);
		break;
	case orientation::pos_z:
	case orientation::neg_z:
		return glm::dvec3(vec.x, vec.y, 0.);
		break;
	}
	ORK_THROW(ORK("Unreachable"));
}
glm::dvec3 point_on_plane(const glm::dvec3&vec, const GLM::dunit3&normal) {
	const double dot = glm::dot(vec, normal.get());
	const glm::dvec3 parallel_component = dot*normal;
	return vec - parallel_component;//The only thing remaining is the orthogonal component
}


bool points_advance_clockwise(const glm::dvec2&begin, const glm::dvec2&end, const glm::dvec2&center) {
	const glm::dvec2 dir_begin(begin - center);
	const glm::dvec2 dir_end(end - center);
	if(GLM::equal(glm::length2(dir_begin), 0.) || GLM::equal(glm::length2(dir_end), 0.)) {
		ORK_THROW(ORK("Cannot compute the angle between a center and itself"));
	}

	const double rad = glm::orientedAngle(glm::normalize(dir_begin), glm::normalize(dir_end));
	return rad < 0.;
}
bool points_advance_clockwise(const glm::dvec3&begin, const glm::dvec3&end, const glm::dvec3&center, const GLM::dunit3&normal) {
	const glm::dvec3 start_dir(glm::normalize(begin - center));
	const glm::dvec3 end_dir(glm::normalize(end - center));
	if(GLM::equal(glm::length2(start_dir), 0.) || GLM::equal(glm::length2(end_dir), 0.)) {
		ORK_THROW(ORK("Cannot compute the angle between a center and itself"));
	}
	const glm::dvec3 cross(glm::cross(start_dir, end_dir));
	return glm::dot(cross, normal.get()) < 0.;
}


double point_on_axis(const glm::dvec3&pos, orientation axis) {
	switch(axis) {
	case orientation::pos_x:
		return pos.x;
	case orientation::neg_x:
		return -pos.x;
	case orientation::pos_y:
		return pos.y;
	case orientation::neg_y:
		return -pos.y;
	case orientation::pos_z:
		return pos.z;
	case orientation::neg_z:
		return -pos.z;
	}
	ORK_THROW(ORK("Unreachable"));
}
double point_on_axis(const glm::dvec3&pos, const GLM::dunit3&axis) {
	return glm::dot(pos, axis.get());//This function serves little more than to not have to remember the dot operator
}
double point_on_axis(const glm::dvec3&pos, const glm::dvec3&axis) {
	return glm::dot(pos, glm::normalize(axis));//This function serves little more than to not have to remember the dot operator
}


bounding_box::bounding_box(const glm::dvec3&v1, const glm::dvec3&v2) :_min_pt(glm::min(v1, v2)), _max_pt(glm::max(v1, v2)) {}


glm::dvec3 bounding_box::mid_pt() const {
	return glm::dvec3(
		(_min_pt.x + _max_pt.x) / 2.0,
		(_min_pt.y + _max_pt.y) / 2.0,
		(_min_pt.z + _max_pt.z) / 2.0
	);
}


double bounding_box::mid_pt(const orientation axis) const {
	return point_on_axis(mid_pt(), axis);
}
double bounding_box::mid_pt(const glm::dvec3&direction) const {
	return point_on_axis(mid_pt(), direction);
}
double bounding_box::mid_pt(const GLM::dunit3&direction)const {
	return point_on_axis(mid_pt(), direction);
}


double bounding_box::max_corner_distance()const {
	const glm::dvec3 displacement = _max_pt - _min_pt;
	return glm::length(displacement);
}


double bounding_box::range_on_axis(const orientation axis)const {
	switch(axis) {
	case orientation::pos_x:
	case orientation::neg_x:
		return _max_pt.x - _min_pt.x;
	case orientation::pos_y:
	case orientation::neg_y:
		return _max_pt.y - _min_pt.y;
	case orientation::pos_z:
	case orientation::neg_z:
		return _max_pt.z - _min_pt.z;
	}
	ORK_THROW(ORK("Unreachable"));
}


bool operator==(const bounding_box&lhs, const bounding_box&rhs) {
	return equal(lhs.min_pt(), rhs.min_pt()) && equal(lhs.max_pt(), rhs.max_pt());
}


interval project_box_on_axis(const glm::dvec3&p1, const glm::dvec3&p2, const GLM::dunit3&axis) {
	//Test all 8 corners and return the min, max.  This is surely not the beast way to do this
	double minp = std::numeric_limits<double>::infinity();
	double maxp = -std::numeric_limits<double>::infinity();
	std::vector<double>xs;
	xs.push_back(p1.x);
	xs.push_back(p2.x);
	std::vector<double>ys;
	ys.push_back(p1.y);
	ys.push_back(p2.y);
	std::vector<double>zs;
	zs.push_back(p1.z);
	zs.push_back(p2.z);

	for(double x : xs)for(double y : ys)for(double z : zs) {
		const glm::dvec3 corner(x, y, z);
		const double projection = glm::dot(corner, axis.get());
		if(projection < minp)minp = projection;
		if(projection > maxp)maxp = projection;
	}
	return interval(minp, maxp);
}


double bounding_box::range_on_axis(const glm::dvec3&direction)const {
	return project_box_on_axis(_max_pt, _min_pt, GLM::dunit3(direction)).length();
}


double bounding_box::range_on_axis(const GLM::dunit3&direction)const {
	return project_box_on_axis(_max_pt, _min_pt, direction).length();
}


double bounding_box::limit_on_axis(const orientation axis)const {
	switch(axis) {
	case orientation::pos_x:
		return _max_pt.x;
	case orientation::neg_x:
		return -_min_pt.x;
	case orientation::pos_y:
		return _max_pt.y;
	case orientation::neg_y:
		return -_min_pt.y;
	case orientation::pos_z:
		return _max_pt.z;
	case orientation::neg_z:
		return -_min_pt.z;
	}
	ORK_THROW(ORK("Unreachable"));
}


double bounding_box::limit_on_axis(const glm::dvec3&direction)const {
	const interval limits = project_box_on_axis(_min_pt, _max_pt, GLM::dunit3(direction));
	return limits.max();
}
double bounding_box::limit_on_axis(const GLM::dunit3&direction)const {
	const interval limits = project_box_on_axis(_min_pt, _max_pt, direction);
	return limits.max();
}


bool bounding_box::strictly_contained_in(const bounding_box&other)const {
	return greater(_min_pt, other._min_pt) && less(_max_pt, other._max_pt);
}
bool bounding_box::strictly_contains(const bounding_box&other)const {
	return less(_min_pt, other._min_pt) && greater(_max_pt, other._max_pt);
}
bool bounding_box::strictly_overlaps(const bounding_box&other)const {
	return	greater(_min_pt, other._min_pt) && greater(_max_pt, other._max_pt) ||
		less(_min_pt, other._min_pt) && less(_max_pt, other._max_pt);
}
bool bounding_box::weakly_contained_in(const bounding_box&other)const {
	return greater_equal(_min_pt, other._min_pt) && less_equal(_max_pt, other._max_pt);
}
bool bounding_box::weakly_contains(const bounding_box&other)const {
	return less_equal(_min_pt, other._min_pt) && greater_equal(_max_pt, other._max_pt);
}
bool bounding_box::weakly_overlaps(const bounding_box&other)const {
	return !strictly_contained_in(other) && !strictly_contains(other);
}
double bounding_box::shortest_side()const {
	return std::min(std::min(_max_pt.x - _min_pt.x, _max_pt.y - _min_pt.y), _max_pt.z - _min_pt.z);
}
double bounding_box::volume()const {
	return (_max_pt.x - _min_pt.x)*(_max_pt.y - _min_pt.y)*(_max_pt.z - _min_pt.z);
}


void bounding_box::grow(const orientation axis, double depth) {
	switch(axis) {
	case orientation::pos_x:
		_max_pt.x = std::max(_max_pt.x, depth);
		break;
	case orientation::neg_x:
		_min_pt.x = std::min(_min_pt.x, -depth);
		break;
	case orientation::pos_y:
		_max_pt.y = std::max(_max_pt.y, depth);
		break;
	case orientation::neg_y:
		_min_pt.y = std::min(_min_pt.y, -depth);
		break;
	case orientation::pos_z:
		_max_pt.z = std::max(_max_pt.z, depth);
		break;
	case orientation::neg_z:
		_min_pt.z = std::min(_min_pt.z, -depth);
		break;
	}
}


void bounding_box::shrink(const orientation axis, double depth) {
	switch(axis) {
	case orientation::pos_x:
		_max_pt.x = std::max(_min_pt.x, depth);
		break;
	case orientation::neg_x:
		_min_pt.x = std::min(_max_pt.x, -depth);
		break;
	case orientation::pos_y:
		_max_pt.y = std::max(_min_pt.y, depth);
		break;
	case orientation::neg_y:
		_min_pt.y = std::min(_max_pt.y, -depth);
		break;
	case orientation::pos_z:
		_max_pt.z = std::max(_min_pt.z, depth);
		break;
	case orientation::neg_z:
		_min_pt.z = std::min(_max_pt.z, -depth);
		break;
	}
}


void bounding_box::resize(const orientation axis, double depth) {
	grow(axis, depth);
	shrink(axis, depth);
}


bounding_box operator|(const bounding_box&lhs, const bounding_box&rhs) {
	return bounding_box(glm::min(lhs.min_pt(), rhs.min_pt()), glm::max(lhs.max_pt(), rhs.max_pt()));
}


string bounding_box2string(const bounding_box&box) {
	string_stream strm;
	strm << ORK("[(") << box.min_pt().x << ORK(", ") << box.min_pt().y << ORK(", ") << box.min_pt().z << ORK("), (") << box.max_pt().x << ORK(", ") << box.max_pt().y << ORK(", ") << box.max_pt().z << ORK(")]");
	return strm.str();
}


o_stream&operator<<(o_stream&strm, const bounding_box&box) {
	strm << bounding_box2string(box);
	return strm;
}


}//namespace GLM
}//namespace ork
