/*
This file is part of the PSU ARL Weld Analysis Kit developed by
The Pennsylvania State University Applied Research Laboratory.  Full
copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/glm.hpp"

#if ORK_USE_PUGI
#include"pugixml.hpp"
#endif


namespace ork {
namespace xml {


class exportable {
public:
	virtual ~exportable() {}//To support polymorphic hierarchies of nodes
public:
#if ORK_USE_PUGI
	virtual void export_xml(pugi::xml_node &n) const = 0;
#endif
};


class vector : public exportable {
protected:
	glm::dvec3 _data;
public:
	vector() : _data() {}
	/*implicit*/vector(const glm::dvec3&vec) : _data(vec) {}
	vector(const double x, const double y, const double z) : _data(x, y, z) {}
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
#if ORK_USE_PUGI
	explicit vector(pugi::xml_node &node);
	virtual void export_xml(pugi::xml_node &n) const;
#endif
};


}//namespace xml
}//namespace ork
