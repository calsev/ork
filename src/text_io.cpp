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


void export_file(const string& path_to_file, const node& root)
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
    node root;
    object.to_json(root);
    export_file(path_to_file, root);
}
void load_and_parse(bi_stream& fin, node& root)
{
    fin >> root;
}
node load_and_parse(bi_stream& fin)
{
    node root;
    fin >> root;
    return std::move(root);
}


#endif // ORK_USE_JSON


} // namespace json
namespace xml {


#if ORK_USE_PUGI


void export_object(pugi::xml_document& doc, const bstring& root_tag, const exportable& object)
{
    const tagged* const tag_ptr = dynamic_cast<const tagged*>(&object);
    if(tag_ptr == nullptr) {
        node root_node = doc.append_child(root_tag.c_str());
        object.to_xml(root_node);
    }
    else {
        object.to_xml(doc);
    }
}


void export_file(const string& path_to_file, const exportable& object, const bstring& root_tag)
{
    pugi::xml_document doc;
    auto decl_node = doc.append_child(pugi::node_declaration);
    decl_node.append_attribute("version") = BORK("1.0");
    decl_node.append_attribute("encoding") = BORK("utf-8");
    // decl_node.append_attribute("standalone") = BORK("yes");
    export_object(doc, root_tag, object);
    file::ensure_directory(path_to_file);
    ORK_FILE_WRITE_B(path_to_file);
    doc.save(fout, BORK("\t"), pugi::format_default, pugi::encoding_utf8);
}
void export_file_permissive(const string& path_to_file, const bstring& root_tag, const exportable& object)
{
    if(!file::ensure_directory(path_to_file)) {
        ORK_LOG(ork::severity_level::error)
            << ORK("Could not create XML directory: ") << path_to_file;
    }
    else {
        ORK_LOG(ork::severity_level::trace)
            << ORK("Creating XML file: ") << path_to_file;
        try {
            ORK_FILE_WRITE_B(path_to_file);
            pugi::xml_document doc;
            export_object(doc, root_tag, object);
            doc.save(fout, BORK("\t"), pugi::format_default, pugi::encoding_utf8);
        }
        catch(ork::exception& e) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Exception writing XML file: ") << e.what();
        }
        catch(...) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Something happened while writing XML file");
        }
    }
}
void load_and_parse(bi_stream& fin, pugi::xml_document& xml)
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


#    define ORK_XML_IMPORT(NODE) \
        try { \
            object.from_xml(NODE); \
        } \
        catch(ork::exception & e) { \
            ORK_LOG(ork::severity_level::error) \
                << ORK("Exception parsing XML file: ") << e.what(); \
        } \
        catch(...) { \
            ORK_LOG(ork::severity_level::error) \
                << ORK("Something happened while parsing XML file"); \
        }


void import_object(const pugi::xml_document& doc, const ork::bstring& root_tag, importable& object)
{
    const tagged* const tag_ptr = dynamic_cast<const tagged*>(&object);
    if(tag_ptr == nullptr) {
        const node root_node{doc.child(root_tag.c_str())};
        ORK_XML_IMPORT(root_node)
    }
    else {
        ORK_XML_IMPORT(doc)
    }
}
void load_and_parse_permissive(
    const ork::file::path& path_to_file,
    const ork::bstring& root_tag,
    importable& object)
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
            ORK_FILE_READ_B(path_to_file);
            load_and_parse(fin, doc);
        }
        catch(ork::exception& e) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Exception loading XML file: ") << e.what();
        }
        catch(...) {
            ORK_LOG(ork::severity_level::error)
                << ORK("Something happened while loading XML file");
        }
        import_object(doc, root_tag, object);
        ORK_LOG(ork::severity_level::trace) << ORK("Parsing XML");
    }
}


void exportable_to_xml(node& node, const exportable& value)
{
    try {
        value.to_xml(node);
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to set tag ") << ORK_BYTE_2_STR(node.name())
            << ORK(": ") << ORK_BYTE_2_STR(e.what());
    }
    catch(...) {
        ORK_LOG(severity_level::error) << ORK("Something happened setting tag ")
                                       << ORK_BYTE_2_STR(node.name());
    }
}
void exportable_to_xml(node& node, const bstring& tag, const exportable& value)
{
    auto n = node.append_child(tag.c_str());
    exportable_to_xml(n, value);
}


void importable_from_xml(const node& node, importable& value)
{
    try {
        value.from_xml(node);
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to read tag ") << ORK_BYTE_2_STR(node.name())
            << ORK(": ") << ORK_BYTE_2_STR(e.what());
    }
    catch(...) {
        ORK_LOG(severity_level::error) << ORK("Something happened reading tag ")
                                       << ORK_BYTE_2_STR(node.name());
    }
}
void importable_from_xml(const node& node, const bstring& tag, importable& value)
{
    auto n = node.child(tag.c_str());
    importable_from_xml(n, value);
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


void export_file(const string& path_to_file, const node& root)
{
    file::ensure_directory(path_to_file);
    ORK_FILE_WRITE(path_to_file);
    fout << root;
}
void export_file(const string& path_to_file, const exportable& object)
{
    node root;
    object.export_yaml(root);
    export_file(path_to_file, root);
}
void load_and_parse(bi_stream& fin, node& root)
{
    root = YAML::Load(fin);
}
node load_and_parse(bi_stream& fin)
{
    node root = YAML::Load(fin);
    return std::move(root);
}


#endif // ORK_USE_YAML


} // namespace yaml


} // namespace ork
