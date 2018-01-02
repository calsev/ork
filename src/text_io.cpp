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
#include"ork/private/text_includes.hpp"
#include"ork/text_io.hpp"


namespace ork {
namespace json {


#if ORK_USE_JSON


void export_file(const string&path_to_file, const Json::Value&root) {
	file::ensure_directory(path_to_file);
	ORK_FILE_WRITE(path_to_file);

	Json::StreamWriterBuilder write_builder;
	write_builder["indentation"] = "\t";
	bstring document = Json::writeString(write_builder, root);
	fout << document;
}
void export_file(const string&path_to_file, const exportable&object) {
	Json::Value root;
	object.export_json(root);
	export_file(path_to_file, root);
}
void load_and_parse(i_stream&fin, Json::Value&root) {
	fin >> root;
}
Json::Value load_and_parse(i_stream&fin) {
	Json::Value root;
	fin >> root;
	return std::move(root);
}


#endif//ORK_USE_JSON


}//namespace json
namespace xml {


#if ORK_USE_PUGI


void export_file(const string&path_to_file, const exportable&object, const string&root_node_name) {
	pugi::xml_document doc;
	pugi::xml_node root_node = doc.append_child(root_node_name.c_str());
	object.export_xml(root_node);
	file::ensure_directory(path_to_file);
	ORK_FILE_WRITE(path_to_file);
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


#endif//ORK_USE_PUGI


}//namespace xml
namespace yaml {


#if ORK_USE_YAML


void export_file(const string&path_to_file, const YAML::Node&root) {
	file::ensure_directory(path_to_file);
	ORK_FILE_WRITE(path_to_file);
	fout << root;
}
void export_file(const string&path_to_file, const exportable&object) {
	YAML::Node root;
	object.export_yaml(root);
	export_file(path_to_file, root);
}
void load_and_parse(i_stream&fin, YAML::Node&root) {
	root = YAML::Load(fin);
}
YAML::Node load_and_parse(i_stream&fin) {
	YAML::Node root = YAML::Load(fin);
	return std::move(root);
}


#endif//ORK_USE_YAML


}//namespace yaml


}//namespace ork
