/*
This file is part of the PSU ARL Weld Analysis Kit developed by
The Pennsylvania State University Applied Research Laboratory.  Full
copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/glm.hpp"

namespace pugi {
class xml_document; 
class xml_node;
}


namespace ork {
namespace xml {


class exportable {
public:
	virtual ~exportable() {}//To support polymorphic hierarchies of nodes
public:
	virtual void export_xml(pugi::xml_node &n) const = 0;
};


void export_file(const string&filename, const exportable&object, const string&root_node_name);
void load_and_parse(pugi::xml_document&xml, i_stream&fin);//Just create a file with error checking


class vector : public exportable {
protected:
	glm::dvec3 _data;
public:
	vector() : _data() {}
	/*implicit*/vector(const glm::dvec3&vec) : _data(vec) {}
	vector(const double x, const double y, const double z) : _data(x, y, z) {}
	explicit vector(pugi::xml_node &node);
public:
	string as_string() const;

	double&x();
	const double&x()const;

	double&y();
	const double&y()const;

	double&z();
	const double&z()const;

	glm::dvec3&get();
	const glm::dvec3&get()const;

	bool operator == (const vector &other) const;
	inline bool operator != (const vector &other) const {
		return !(*this == other);
	}
	virtual void export_xml(pugi::xml_node &n) const;
};


ORK_INLINE o_stream &operator << (o_stream&stream, const ork::xml::vector &vec) {
	return stream << vec.as_string();
}


}//namespace xml
}//namespace ork
