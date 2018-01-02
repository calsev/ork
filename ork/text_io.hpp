/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/memory.hpp"
#include"glm/fwd.hpp"


namespace Json {
class Value;
}


namespace pugi {
class xml_document;
class xml_node;
}


namespace ork {
namespace json {


#if ORK_USE_JSON


class ORK_ORK_API exportable {
public:
	virtual ~exportable() {}//To support polymorphic hierarchies of nodes
public:
	virtual void export_json(Json::Value& v) const = 0;
};


ORK_ORK_EXT(void) export_file(const string&path_to_file, const Json::Value&root);
ORK_ORK_EXT(void) export_file(const string&path_to_file, const exportable&object);
ORK_ORK_EXT(void) load_and_parse(i_stream&fin, Json::Value&root);
ORK_ORK_EXT(Json::Value) load_and_parse(i_stream&fin);


#endif//ORK_USE_JSON


}//namespace json
namespace xml {


#if ORK_USE_PUGI


class ORK_ORK_API exportable {
public:
	virtual ~exportable() {}//To support polymorphic hierarchies of nodes
public:
	virtual void export_xml(pugi::xml_node &n) const = 0;
};


ORK_ORK_EXT(void) export_file(const string&path_to_file, const exportable&object, const string&root_node_name);
ORK_ORK_EXT(void) load_and_parse(i_stream&fin, pugi::xml_document&root);//Just create a file with error checking


#endif//ORK_USE_PUGI


}//namespace xml


#if ORK_USE_GLM


class vector
#if ORK_USE_PUGI
	: public xml::exportable
#endif
{
private:
	struct impl;
	struct deleter {
		void operator()(const impl*);
		vector::impl operator()(const vector::impl&);
	};
private:
	value_ptr<impl, deleter>_pimpl;
public:
	ORK_ORK_API vector();
	ORK_ORK_API explicit vector(const glm::dvec3&vec);
	ORK_ORK_API explicit vector(const GLM::dunit3&vec);
	ORK_ORK_API vector(const double x, const double y, const double z);
#if ORK_USE_PUGI
	ORK_ORK_API explicit vector(pugi::xml_node &node);
#endif
public:
	ORK_ORK_API vector&operator=(const glm::dvec3&);
	ORK_ORK_API vector&operator=(const GLM::dunit3&);
public:
	ORK_ORK_API double&x();
	ORK_ORK_API const double&x()const;

	ORK_ORK_API double&y();
	ORK_ORK_API const double&y()const;

	ORK_ORK_API double&z();
	ORK_ORK_API const double&z()const;

	ORK_ORK_API glm::dvec3&get();
	ORK_ORK_API const glm::dvec3&get()const;

	ORK_ORK_API bool operator == (const vector &other) const;
	ORK_ORK_API bool operator != (const vector &other) const;

	ORK_ORK_API string as_string() const;
#if ORK_USE_PUGI
	ORK_ORK_API virtual void export_xml(pugi::xml_node &n) const;
#endif
};


ORK_ORK_API o_stream &operator << (o_stream&stream, const ork::vector &vec);


#endif//ORK_USE_GLM


}//namespace ork
