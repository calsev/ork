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


class ORK_ORK_API exportable {
public:
	 virtual ~exportable() {}//To support polymorphic hierarchies of nodes
public:
	virtual void export_xml(pugi::xml_node &n) const = 0;
};


ORK_ORK_EXT(void) export_file(const string&filename, const exportable&object, const string&root_node_name);
ORK_ORK_EXT(void) load_and_parse(pugi::xml_document&xml, i_stream&fin);//Just create a file with error checking


#if ORK_USE_GLM


class vector : public exportable {
private:
	glm::dvec3 _data;
public:
	ORK_INLINE vector() : _data() {}
	/*implicit*/vector(const glm::dvec3&vec) : _data(vec) {}
	ORK_INLINE vector(const double x, const double y, const double z) : _data(x, y, z) {}
	ORK_ORK_API explicit vector(pugi::xml_node &node);
public:
	ORK_ORK_API string as_string() const;

	ORK_ORK_API double&x();
	ORK_ORK_API const double&x()const;

	ORK_ORK_API double&y();
	ORK_ORK_API const double&y()const;

	ORK_ORK_API double&z();
	ORK_ORK_API const double&z()const;

	ORK_ORK_API glm::dvec3&get();
	ORK_ORK_API const glm::dvec3&get()const;

	ORK_ORK_API bool operator == (const vector &other) const;
	ORK_INLINE bool operator != (const vector &other) const {
		return !(*this == other);
	}
	ORK_ORK_API virtual void export_xml(pugi::xml_node &n) const;
};


ORK_INLINE o_stream &operator << (o_stream&stream, const ork::xml::vector &vec) {
	return stream << vec.as_string();
}


#endif//ORK_USE_GLM


}//namespace xml
}//namespace ork
