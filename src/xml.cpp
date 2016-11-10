/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/geometry.hpp"
#include"ork/xml.hpp"

#if ORK_USE_PUGI
#include"pugixml.hpp"
#endif


namespace ork {
namespace xml {


#if ORK_USE_PUGI
void export_file(const string&filename, const exportable&object, const string&root_node_name) {
	pugi::xml_document doc;
	object.export_xml(doc.append_child(root_node_name.c_str()));
	ork::ensure_directory(filename);
	doc.save_file(filename.c_str());
}
void load_and_parse(pugi::xml_document&xml, i_stream&fin) {
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


double&vector::x() {
	return _data.x;
}
const double&vector::x()const {
	return _data.x;
}

double&vector::y() {
	return _data.y;
}
const double&vector::y()const {
	return _data.y;
}

double&vector::z() {
	return _data.z;
}
const double&vector::z()const {
	return _data.z;
}

glm::dvec3&vector::get() {
	return _data;
}
const glm::dvec3&vector::get()const {
	return _data;
}


string vector::as_string() const {
	string_stream stream;
	stream << ORK("(") << to_dimension(_data.x)
		<< ORK(", ") << to_dimension(_data.y)
		<< ORK(", ") << to_dimension(_data.z) << ORK(")");
	return stream.str();
}

bool vector::operator == (const vector &other) const {
	static const double tolerance = 0.0001;//TODO: This is problematic, given lack of units (e.g. meters)

	if(std::abs(_data.x - other._data.x) > tolerance) return false;
	if(std::abs(_data.y - other._data.y) > tolerance) return false;
	if(std::abs(_data.z - other._data.z) > tolerance) return false;
	return true;
}


#if ORK_USE_PUGI
vector::vector(pugi::xml_node &node) {
	_data.x = node.attribute(ORK("x")).as_double();
	_data.y = node.attribute(ORK("y")).as_double();
	_data.z = node.attribute(ORK("z")).as_double();
}
void vector::export_xml(pugi::xml_node &node) const {
	node.append_attribute(ORK("x"));
	node.attribute(ORK("x")).set_value(to_dimension(_data.x).c_str());

	node.append_attribute(ORK("y"));
	node.attribute(ORK("y")).set_value(to_dimension(_data.y).c_str());

	node.append_attribute(ORK("z"));
	node.attribute(ORK("z")).set_value(to_dimension(_data.z).c_str());
}
#endif


}//namespace xml
}//namespace ork
