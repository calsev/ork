/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/memory.hpp"

#include"glm/fwd.hpp"


#if ORK_USE_JSON
namespace Json {
class Value;
}
#endif


#if ORK_USE_PUGI
namespace pugi {
class xml_document;
class xml_node;
}
#endif


#if ORK_USE_YAML
namespace YAML {
class Node;
}
#endif


namespace ork {
namespace json {


#if ORK_USE_JSON


class ORK_ORK_API exportable {
public:
	virtual ~exportable() {}
public:
	virtual void export_json(Json::Value& v) const = 0;
};


class ORK_ORK_API serializable {
public:
	virtual ~serializable() {}
public:
	virtual void to_json(Json::Value& v) const = 0;
	virtual void from_json(const Json::Value& v) = 0;
};


ORK_ORK_EXT(void) export_file(const string&path_to_file, const Json::Value&root);
ORK_ORK_EXT(void) export_file(const string&path_to_file, const exportable&object);
ORK_ORK_EXT(void) load_and_parse(bi_stream&fin, Json::Value&root);
ORK_ORK_EXT(Json::Value) load_and_parse(bi_stream&fin);


#endif//ORK_USE_JSON


}//namespace json
namespace xml {


#if ORK_USE_PUGI


class ORK_ORK_API exportable {
public:
	virtual ~exportable() {}
public:
	virtual void export_xml(pugi::xml_node &n) const = 0;
};


class ORK_ORK_API serializable {
public:
	virtual ~serializable() {}
public:
	virtual void to_xml(pugi::xml_node& v) const = 0;
	virtual void from_xml(const pugi::xml_node& v) = 0;
};


ORK_ORK_EXT(void) export_file(const string&path_to_file, const exportable&object, const string&root_node_name);
ORK_ORK_EXT(void) load_and_parse(i_stream&fin, pugi::xml_document&root);//Just create a file with error checking


#endif//ORK_USE_PUGI


}//namespace xml
namespace yaml {


#if ORK_USE_YAML


class ORK_ORK_API exportable {
public:
	virtual ~exportable() {}
public:
	virtual void export_yaml(YAML::Node& n) const = 0;
};


class ORK_ORK_API serializable {
public:
	virtual ~serializable() {}
public:
	virtual void to_yaml(YAML::Node& v) const = 0;
	virtual void from_yaml(const YAML::Node& v) = 0;
};


ORK_ORK_EXT(void) export_file(const string&path_to_file, const YAML::Node&root);
ORK_ORK_EXT(void) export_file(const string&path_to_file, const exportable&object);
ORK_ORK_EXT(void) load_and_parse(bi_stream&fin, YAML::Node&root);
ORK_ORK_EXT(YAML::Node) load_and_parse(bi_stream&fin);


#endif//ORK_USE_YAML


}//namespace yaml
}//namespace ork
