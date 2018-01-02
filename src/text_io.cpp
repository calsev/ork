/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<fstream>
#include<iostream>
#include<sstream>

#include"ork/glm.hpp"
#include ORK_FILE_INCLUDE

#include"ork/geometry.hpp"
#include"ork/text_io.hpp"

#if ORK_USE_PUGI
#include"pugixml.hpp"
#endif

#if ORK_USE_JSON
#include"json/json.h"
#endif


namespace ork {
namespace xml {


#if ORK_USE_PUGI

void export_file(const string&filename, const exportable&object, const string&root_node_name) {
	pugi::xml_document doc;
	pugi::xml_node root_node = doc.append_child(root_node_name.c_str());
	object.export_xml(root_node);
	file::ensure_directory(filename);
	ORK_FILE_WRITE(filename);
	doc.save(fout);
}
void load_and_parse(i_stream&fin, pugi::xml_document&xml) {
	pugi::xml_parse_result result = xml.load(fin);
	if(!result) {
		ORK_THROW(ORK("XML parse error") \
			<< ORK("\n -- Error: ") << result.description() \
			<< ORK("\n -- Offset: ") << result.offset)//<< ORK(" at [") << (source + result.offset) << ORK("]"))
	}
	else {
		ORK_LOG(ork::severity_level::trace) << ORK("XML parse success");
	}
}

#endif


#if ORK_USE_GLM


struct vector::impl {
public:
	glm::dvec3 data;
public:
	impl() : data{ } {}
	explicit impl(const glm::dvec3&vec) : data{ vec } {}
	impl(const double x, const double y, const double z) : data{ x, y, z } {}
};


void vector::deleter::operator()(const vector::impl*ptr) {
	delete ptr;
}
vector::impl vector::deleter::operator()(const vector::impl&val) {
	return impl(val);
}


vector::vector() : _pimpl{ new impl() } {}
vector::vector(const glm::dvec3&vec) : _pimpl{ new impl(vec) } {}
vector::vector(const GLM::dunit3&vec) : _pimpl{ new impl(vec.get()) } {}
vector::vector(const double x, const double y, const double z) : _pimpl{ new impl(x, y, z) } {}
#if ORK_USE_PUGI
vector::vector(pugi::xml_node &node) {
	_pimpl->data.x = node.attribute(ORK("x")).as_double();
	_pimpl->data.y = node.attribute(ORK("y")).as_double();
	_pimpl->data.z = node.attribute(ORK("z")).as_double();
}
#endif


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
#if ORK_USE_PUGI
void vector::export_xml(pugi::xml_node &node) const {
	node.append_attribute(ORK("x")).set_value(to_dimension(_pimpl->data.x).c_str());
	node.append_attribute(ORK("y")).set_value(to_dimension(_pimpl->data.y).c_str());
	node.append_attribute(ORK("z")).set_value(to_dimension(_pimpl->data.z).c_str());
}
#endif


o_stream &operator << (o_stream&stream, const ork::xml::vector &vec) {
	return stream << vec.as_string();
}


#endif//ORK_USE_GLM


}//namespace xml


namespace json {


#if ORK_USE_JSON

void export_file(const string&filename, const Json::Value&root) {
	ork::ensure_directory(filename);
	ORK_FILE_WRITE(filename);

	Json::StyledWriter writer;
	fout << writer.write(root);
}
void export_file(const string&filename, const exportable&object) {
	Json::Value root;
	object.export_json(root);
	export_file(filename, root);
}
void load_and_parse(i_stream&fin, Json::Value&root) {
	fin >> root;
}

#endif


}//namespace json
}//namespace ork
