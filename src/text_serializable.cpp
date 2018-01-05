/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#define ORK_STL_INC_FILE <sstream>
#include"ork/core/stl_include.inl"

#include"ork/geometry.hpp"
#include"ork/private/text_includes.hpp"
#include"ork/text_serializable.hpp"


namespace ork {


#if ORK_USE_GLM


struct vector::impl {
public:
	glm::dvec3 data;
public:
	impl() : data{} {}
	explicit impl(const glm::dvec3&vec) : data{vec} {}
	impl(const double x, const double y, const double z) : data{x, y, z} {}
};


void vector::deleter::operator()(const vector::impl*ptr) {
	delete ptr;
}
vector::impl vector::deleter::operator()(const vector::impl&val) {
	return impl(val);
}


vector::vector() : _pimpl{new impl()} {}
vector::vector(const glm::dvec3&vec) : _pimpl{new impl(vec)} {}
vector::vector(const GLM::dunit3&vec) : _pimpl{new impl(vec.get())} {}
vector::vector(const double x, const double y, const double z) : _pimpl{new impl(x, y, z)} {}


vector&vector::operator=(const glm::dvec3&vec) {
	_pimpl->data = vec;
	return *this;
}
vector&vector::operator=(const GLM::dunit3&vec) {
	_pimpl->data = vec.get();
	return *this;
}

double&vector::x() {
	return _pimpl->data.x;
}
const double&vector::x()const {
	return _pimpl->data.x;
}

double&vector::y() {
	return _pimpl->data.y;
}
const double&vector::y()const {
	return _pimpl->data.y;
}

double&vector::z() {
	return _pimpl->data.z;
}
const double&vector::z()const {
	return _pimpl->data.z;
}

glm::dvec3&vector::get() {
	return _pimpl->data;
}
const glm::dvec3&vector::get()const {
	return _pimpl->data;
}


bool vector::operator == (const vector &other) const {
	return GLM::equal(_pimpl->data, other._pimpl->data);
}
bool vector::operator != (const vector &other) const {
	return !(*this == other);
}


string vector::as_string() const {
	string_stream stream;
	stream << ORK("(") << to_dimension(_pimpl->data.x)
		<< ORK(", ") << to_dimension(_pimpl->data.y)
		<< ORK(", ") << to_dimension(_pimpl->data.z) << ORK(")");
	return stream.str();
}


#if ORK_USE_JSON
vector::vector(Json::Value &node) : vector{} {
	_pimpl->data.x = node["x"].asDouble();
	_pimpl->data.y = node["y"].asDouble();
	_pimpl->data.z = node["z"].asDouble();
}
void vector::export_json(Json::Value &n) const {
	n["x"] = _pimpl->data.x;
	n["y"] = _pimpl->data.y;
	n["z"] = _pimpl->data.z;
}
#endif
#if ORK_USE_PUGI
vector::vector(pugi::xml_node &node) : vector{} {
	_pimpl->data.x = node.attribute(BORK("x")).as_double();
	_pimpl->data.y = node.attribute(BORK("y")).as_double();
	_pimpl->data.z = node.attribute(BORK("z")).as_double();
}
void vector::export_xml(pugi::xml_node &node) const {
	node.append_attribute(BORK("x")).set_value(ORK_STR_2_BYTE(to_dimension(_pimpl->data.x)).c_str());
	node.append_attribute(BORK("y")).set_value(ORK_STR_2_BYTE(to_dimension(_pimpl->data.y)).c_str());
	node.append_attribute(BORK("z")).set_value(ORK_STR_2_BYTE(to_dimension(_pimpl->data.z)).c_str());
}
#endif
#if ORK_USE_YAML
vector::vector(YAML::Node &node) : vector{} {
	_pimpl->data.x = node["x"].as<double>();
	_pimpl->data.y = node["y"].as<double>();
	_pimpl->data.z = node["z"].as<double>();
}
void vector::export_yaml(YAML::Node &n) const {
	n["x"] = _pimpl->data.x;
	n["y"] = _pimpl->data.y;
	n["z"] = _pimpl->data.z;
}
#endif


o_stream &operator << (o_stream&stream, const ork::vector &vec) {
	return stream << vec.as_string();
}


#endif//ORK_USE_GLM


}//namespace ork
