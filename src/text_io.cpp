/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/ork.hpp"
#define ORK_STL_INC_FILE <fstream>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <iostream>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include "ork/core/stl_include.inl"

#include "ork/glm.hpp"

#include "ork/geometry.hpp"
#include "ork/private/text_includes.hpp"
#include "ork/text_io.hpp"


namespace ork {
namespace json {


#if ORK_USE_JSON


void export_file(const string& path_to_file, const Json::Value& root)
{
    file::ensure_directory(path_to_file);
    ORK_FILE_WRITE_B(path_to_file);

    Json::StreamWriterBuilder write_builder;
    write_builder["indentation"] = "\t";
    bstring document = Json::writeString(write_builder, root);
    fout << document;
}
void export_file(const string& path_to_file, const exportable& object)
{
    Json::Value root;
    object.export_json(root);
    export_file(path_to_file, root);
}
void load_and_parse(bi_stream& fin, Json::Value& root)
{
    fin >> root;
}
Json::Value load_and_parse(bi_stream& fin)
{
    Json::Value root;
    fin >> root;
    return std::move(root);
}


#endif // ORK_USE_JSON


} // namespace json
namespace xml {


#if ORK_USE_PUGI


void export_file(const string& path_to_file, const exportable& object, const string& root_node_name)
{
    pugi::xml_document doc;
    pugi::xml_node root_node = doc.append_child(ORK_STR_2_BYTE(root_node_name).c_str());
    object.export_xml(root_node);
    file::ensure_directory(path_to_file);
    ORK_FILE_WRITE(path_to_file);
    doc.save(fout);
}
void load_and_parse(i_stream& fin, pugi::xml_document& xml)
{
    pugi::xml_parse_result result = xml.load(fin);
    if(!result) {
        ORK_THROW(
            ORK("XML parse error")
            << ORK("\n -- Error: ") << result.description() << ORK("\n -- Offset: ")
            << result.offset) //<< ORK(" at [") << (source + result.offset) << ORK("]"))
    }
    else {
        ORK_LOG(ork::severity_level::trace) << ORK("XML parse success");
    }
}
void load_and_parse_permissive(
    const ork::file::path& path_to_file,
    const ork::bstring& root_tag,
    ork::xml::serializable& obj)
{
    if(!ork::ext_file::is_regular_file(path_to_file)) {
        ORK_LOG(ork::severity_level::error)
            << ORK("Could not find XML file: ") << path_to_file.ORK_GEN_STR();
    }
    else {
        ORK_LOG(ork::severity_level::trace)
            << ORK("Loading XML file: ") << path_to_file.ORK_GEN_STR();

        pugi::xml_document doc{};
        try {
            ORK_FILE_READ(path_to_file);
            ork::xml::load_and_parse(fin, doc);
        }
        catch(ork::exception& e) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Exception loading XML file: ") << e.what();
        }
        catch(...) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Something happened while loading XML file");
        }

        ORK_LOG(ork::severity_level::trace) << ORK("Parsing XML");
        auto root_node{doc.child(root_tag.c_str())};
        if(!root_node) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Root node not found: ") << ORK_BYTE_2_STR(root_tag);
        }
        else {
            ORK_LOG(ork::severity_level::trace) << ORK("Parsing XML object");
            try {
                obj.from_xml(root_node);
            }
            catch(ork::exception& e) {
                ORK_LOG(ork::severity_level::error)
                    << ORK("Exception parsing XML file: ") << e.what();
            }
            catch(...) {
                ORK_LOG(ork::severity_level::error)
                    << ORK("Something happened while parsing XML file");
            }
        }
    }
}


/*
These are permissive interfaces
*/
ORK_XML_SERIALIZE_DEF(int)
ORK_XML_SERIALIZE_DEF(unsigned)
ORK_XML_SERIALIZE_DEF(size_t)
ORK_XML_SERIALIZE_DEF(float)
ORK_XML_SERIALIZE_DEF(double)
ORK_XML_SERIALIZE_DEF(ork::bstring)
ORK_XML_SERIALIZE_DEF(ork::wstring)


#endif // ORK_USE_PUGI


} // namespace xml
namespace yaml {


#if ORK_USE_YAML


void export_file(const string& path_to_file, const YAML::Node& root)
{
    file::ensure_directory(path_to_file);
    ORK_FILE_WRITE(path_to_file);
    fout << root;
}
void export_file(const string& path_to_file, const exportable& object)
{
    YAML::Node root;
    object.export_yaml(root);
    export_file(path_to_file, root);
}
void load_and_parse(bi_stream& fin, YAML::Node& root)
{
    root = YAML::Load(fin);
}
YAML::Node load_and_parse(bi_stream& fin)
{
    YAML::Node root = YAML::Load(fin);
    return std::move(root);
}


#endif // ORK_USE_YAML


} // namespace yaml


} // namespace ork
