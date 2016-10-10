/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#define _USE_MATH_DEFINES//M_PI
#include<cmath>

#include"ork/glm.hpp"

#include"glm/gtx/rotate_vector.hpp"
#include"glm/gtx/vector_angle.hpp"


namespace ork {


namespace GLM {

o_stream&operator<<(o_stream&stream, const dunit3&vec) {
	return stream << vec.get();
}

const dunit3 pos_x(1.0, 0.0, 0.0);
const dunit3 neg_x(-1., 0.0, 0.0);
const dunit3 pos_y(0.0, 1.0, 0.0);
const dunit3 neg_y(0.0, -1., 0.0);
const dunit3 pos_z(0.0, 0.0, 1.0);
const dunit3 neg_z(0.0, 0.0, -1.);


glm::dvec3 rotate(const glm::dvec3&vec, const dunit3&normal, const dunit3&new_normal) {
	if(GLM::parallel(normal, new_normal))return vec;//Special case, needed to prevent undefined cross product
	if(GLM::antiparallel(normal, new_normal)) {//Special case, needed to prevent undefined cross product
											   //If antiparallel, we rotate about previous axis, in order x, y, z.
											   //TODO ORTHO: Obviously this is not general
		if(GLM::axial(normal, pos_x)) {
			return glm::rotate(vec, M_PI, pos_z.get());
		}
		if(GLM::axial(normal, pos_y)) {
			return glm::rotate(vec, M_PI, pos_x.get());
		}
		if(GLM::axial(normal, pos_z)) {
			return glm::rotate(vec, M_PI, pos_y.get());
		}
		ORK_THROW(ORK("Only handling orthogonal vectors for now"));
	}

	const glm::dvec3 right = glm::normalize(glm::cross(normal.get(), new_normal.get()));
	const glm::dvec3 left = glm::normalize(glm::cross(new_normal.get(), normal.get()));
	if(!equal(right, -left))ORK_THROW(ORK("Orthogonal vector calculation was not stable"));

	const double angle = glm::orientedAngle(normal.get(), new_normal.get(), right);//range [0, PI]
	const glm::dvec3 retval = glm::rotate(vec, angle, right);

	const glm::dmat4x4 rotation = glm::orientation(new_normal.get(), normal.get());
	const glm::dvec3 oriented(rotation * glm::dvec4(vec, 0.));
	//LookAt produces inconsistent results for antiparallel normals (rotates both x and y vectors)
	//const glm::dmat4x4 look_at = glm::lookAt(glm::dvec3(0., 0., 0.), new_normal, normal);

	//This is a temporary check to make sure we understand these functions
	if(!equal(retval, oriented))ORK_THROW(ORK("Rotation and orientation were not equivalent"));

	return retval;
}

glm::dvec3 proj_on_plane(const glm::dvec3&vec, const GLM::dunit3&normal) {
	const glm::dvec3 right = glm::normalize(cross(vec, normal.get()));
	const glm::dvec3 left = glm::normalize(cross(normal.get(), vec));
	if(!equal(right, -left))ORK_THROW(ORK("Orthogonal vector calculation was not stable"));
	const glm::dvec3 l_forward = glm::normalize(cross(normal.get(), left));
	const glm::dvec3 r_forward = glm::normalize(cross(right, normal.get()));
	if(!equal(l_forward, r_forward))ORK_THROW(ORK("Forward vector calculation was not stable"));
	return glm::dot(vec, l_forward) * l_forward;
}

const dunit3&orientation2direction(orientation axis) {
	switch(axis) {
	case orientation::pos_x:
		return pos_x;
	case orientation::neg_x:
		return neg_x;
	case orientation::pos_y:
		return pos_y;
	case orientation::neg_y:
		return neg_y;
	case orientation::pos_z:
		return pos_z;
	case orientation::neg_z:
		return neg_z;
	};

	ORK_THROW(ORK("Unreachable"));
}

}//namespace glm


string to_string(const glm::dvec3&vec) {
	string_stream stream;
	stream << vec;
	return stream.str();
}

}//namespace ork


namespace glm {

ork::o_stream&operator<<(ork::o_stream&stream, const glm::dvec2&vec) {
	stream << ORK("(") << vec.x << ORK(", ") << vec.y << ORK(")");
	return stream;
}
ork::o_stream&operator<<(ork::o_stream&stream, const glm::dvec3&vec) {
	stream << ORK("(") << vec.x << ORK(", ") << vec.y << ORK(", ") << vec.z << ORK(")");
	return stream;
}

}//namespace glm
