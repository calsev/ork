/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include<array>
#include<vector>
#include"ork/ork.hpp"
#include"ork/glm.hpp"


namespace ork {

double mm2inch(double mm);

//Given an angle in radians, return that angle in degrees
double rad2deg(double rad);

//Given an angle in degrees, return that angle in radians
double deg2rad(double deg);

//Given an angle in Radians, return the equivalent angle on [0, 2*pi]
double simple_angle(double angle);

string to_dimension(const double coord);//Fixed point for export


enum class rotation_direction {
	clockwise,
	counter_clockwise
};

rotation_direction operator-(const rotation_direction dir);

enum class angle {
	  radian
	, degree
};

template<angle>struct circle;
template<>struct circle<angle::radian> {
	static const double full;
};
template<>struct circle<angle::degree> {
	static const double full;
};


//Returns true if start->mid->end is in the clockwise direction
//Catches errors related to periodic angles and wrapping at 0
template<angle A>
bool angles_advance_clockwise(const double start_angle, double mid_angle, double end_angle) {
	//Put mid_angle/end_angle in the range [start_angle, start_angle + 2*PI)
	while(mid_angle < start_angle)						mid_angle += circle<A>::full;
	while(mid_angle > start_angle + circle<A>::full)	mid_angle -= circle<A>::full;

	while(end_angle < start_angle)						end_angle += circle<A>::full;
	while(end_angle > start_angle + circle<A>::full)	end_angle -= circle<A>::full;

	/*
	Increasing angles go counterclockwise, so we can now be sure that if mid is smaller, then start-mid-end is counterclockwise.
	If, however, end is smaller, then start-end-mid is counterclockwise, thus start-mid-end is clockwise.
	*/
	LOG(severity_level::trace) << TXT("start_angle=") << start_angle << TXT(", mid_angle=") << mid_angle << TXT(", end_angle=") << end_angle;
	return end_angle <= mid_angle;
}


namespace GLM {
namespace MC {

const glm::dmat3x3&lcs_mat(const orientation axis);
glm::dvec3 to_lcs(const glm::dvec3&vec, const orientation axis);
glm::dvec3 to_lcs(const double val, const orientation axis);

const glm::dmat3x3&wcs_mat(const orientation axis);
glm::dvec3 to_wcs(const glm::dvec3&vec, const orientation axis);
glm::dvec3 to_wcs(const double val, const orientation axis);


//Given an MC standard view, return the directions in the plane.
struct view {
	union {
		std::array<orientation, 4>dirs;
		struct {//Must remain in counter-clockwise order (see rotate)
			orientation right;
			orientation up;
			orientation left;
			orientation down;
		};
	};
	view() :right(orientation::pos_x), up(orientation::pos_y), left(orientation::neg_x), down(orientation::neg_y) {}
	view(const orientation r, const orientation u, const orientation l, const orientation d) :right(r), up(u), left(l), down(d) {}
};

const view&get_view(const orientation axis);

struct rotated_view :public view {
	rotation_direction direction;
	int rotations;
	rotated_view(const rotation_direction dir, const int num) :direction(dir), rotations(num) {}
};


rotated_view rotated(const view&v, const rotation_direction dir, const int rotations);


}//namespace MC


 //These are not projections because we remain 3D, although if we assume RH/LH we could just make them 2D
glm::dvec3 point_on_plane(const glm::dvec3&vec, const orientation axis);
glm::dvec3 point_on_plane(const glm::dvec3&vec, const GLM::dunit3&normal);


bool points_advance_clockwise(const glm::dvec2&begin, const glm::dvec2&end, const glm::dvec2&center);
bool points_advance_clockwise(const glm::dvec3&begin, const glm::dvec3&end, const glm::dvec3&center, const GLM::dunit3&normal);


double point_on_axis(const glm::dvec3&pos, orientation axis);
double point_on_axis(const glm::dvec3&pos, const GLM::dunit3&axis);
double point_on_axis(const glm::dvec3&pos, const glm::dvec3&axis);


class bounding_box {
protected:
	glm::dvec3 _min_pt;
	glm::dvec3 _max_pt;
public:
	bounding_box() : _min_pt(0., 0., 0.), _max_pt(0., 0., 0.) {}
	bounding_box(const glm::dvec3&v1, const glm::dvec3&v2);
	bounding_box(const bounding_box&rhs) : _min_pt(rhs._min_pt), _max_pt(rhs._max_pt) {};
	bounding_box&operator=(const bounding_box&rhs) {
		_min_pt = rhs._min_pt;
		_max_pt = rhs._max_pt;
		return *this;
	}
public:
	const glm::dvec3&min_pt()const {
		return _min_pt;
	}
	const glm::dvec3&max_pt()const {
		return _max_pt;
	}
	glm::dvec3 mid_pt() const;
	double mid_pt(const orientation axis)const;
	double mid_pt(const glm::dvec3&direction)const;
	double mid_pt(const GLM::dunit3&direction)const;
	double max_corner_distance()const;
	double range_on_axis(const orientation axis)const;
	double range_on_axis(const glm::dvec3&direction)const;
	double range_on_axis(const GLM::dunit3&direction)const;
	double limit_on_axis(const orientation axis)const;
	double limit_on_axis(const glm::dvec3&direction)const;
	double limit_on_axis(const GLM::dunit3&direction)const;
	bool strictly_contained_in(const bounding_box&other)const;
	bool strictly_contains(const bounding_box&other)const;
	bool strictly_overlaps(const bounding_box&other)const;//Neither inside nor outside
	bool weakly_contained_in(const bounding_box&other)const;
	bool weakly_contains(const bounding_box&other)const;
	bool weakly_overlaps(const bounding_box&other)const;
	double shortest_side()const;
	double volume()const;
	void grow(const orientation axis, double depth);
	void shrink(const orientation axis, double depth);
	void resize(const orientation axis, double depth);
};

bool operator==(const bounding_box&lhs, const bounding_box&rhs);

bounding_box operator|(const bounding_box&lhs, const bounding_box&rhs);

string bounding_box2string(const bounding_box&box);

o_stream&operator<<(o_stream&strm, const bounding_box&);


//This is a minimal replacement for acis interval
class interval {
protected:
	double _min;
	double _max;
public:
	interval(const double p1, const double p2) :_min(p1 < p2 ? p1 : p2), _max(p1 < p2 ? p2 : p1) {}
public:
	double min()const {
		return _min;
	}
	double max()const {
		return _max;
	}
	double length()const {
		return _max - _min;
	}
};


interval project_box_on_axis(const glm::dvec3&p1, const glm::dvec3&p2, const GLM::dunit3&axis);


//Either unions or a virtual base, we go low-level here
struct segment {
	enum type {
		line,
		arc
	};
	type geometry;
	//'unions' for line and arc
	glm::dvec3 begin_center;
	glm::dvec3 end;
	//Arc
	orientation normal;
	double radius;
	double begin_rad;
	double sweep_rad;
public:
	segment(const type geo) :geometry(geo), begin_center(0.), end(0.), normal(orientation::neg_z), radius(0.), begin_rad(0.), sweep_rad(0.) {}
	segment(const glm::dvec3&begin, const glm::dvec3&end) :geometry(type::line), begin_center(begin), end(end), normal(orientation::neg_z), radius(0.), begin_rad(0.), sweep_rad(0.) {}
	segment(const glm::dvec3&center, const orientation normal, const double rad, const double begin_angle, const double end_angle)
		:geometry(type::arc), begin_center(center), end(0.), normal(normal), radius(rad), begin_rad(begin_angle), sweep_rad(end_angle) {}
};


class chain {
protected:
	std::vector<segment>_segments;
public:
	chain() :_segments() {}
	chain(chain&&other) :_segments(std::move(other._segments)) {}
public:
	chain&operator=(chain&&other) {
		_segments = std::move(other._segments);
		return *this;
	}
	const std::vector<segment>&segments()const { return _segments; }
	std::vector<segment>&segments(){ return _segments; }
	void push_back(const segment&s) { _segments.push_back(s); }
};


}//namespace glm
}//namespace ork
