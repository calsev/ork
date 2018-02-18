/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/memory.hpp"
#include "ork/private/text_includes.hpp"
#include "ork/string_utils.hpp"
#include "ork/type_traits.hpp"

#include "glm/fwd.hpp"


namespace ork {


// Serialization tag: basis for a child node
struct tagged {
private:
    ork::bstring _tag;

public:
    ORK_INLINE tagged(const ork::bstring& tag)
        : _tag{tag}
    {}
    ORK_MOVEABLE(tagged);

public:
    ORK_INLINE const ork::bstring& tag() const
    {
        return _tag;
    }
};


#define ORK_OI_DECL_(SUFFIX) \
    class ORK_ORK_API exportable { \
    public: \
        virtual ~exportable() {} \
\
    public: \
        virtual void ORK_CAT(to_, SUFFIX)(node&) const = 0; \
    }; \
    class ORK_ORK_API importable { \
    public: \
        virtual ~importable() {} \
\
    public: \
        virtual void ORK_CAT(from_, SUFFIX)(const node&) = 0; \
    }; \
    class ORK_ORK_API serializable \
        : public exportable \
        , public importable { \
    public: \
        virtual ~serializable() {} \
    };


namespace json {


#if ORK_USE_JSON


using node = Json::Value;
ORK_OI_DECL_(json)


ORK_ORK_EXT(void)
export_file(const string& path_to_file, const node& root);
ORK_ORK_EXT(void)
export_file(const string& path_to_file, const exportable& object);
ORK_ORK_EXT(void) load_and_parse(bi_stream& fin, node& root);
ORK_ORK_EXT(node) load_and_parse(bi_stream& fin);


#endif // ORK_USE_JSON


} // namespace json
namespace xml {


#if ORK_USE_PUGI


using node = pugi::xml_node;
ORK_OI_DECL_(xml)


ORK_ORK_EXT(void)
export_file(const string& path_to_file, const exportable& object, const bstring& root_tag);
ORK_ORK_EXT(void)
export_file_permissive(const string& path_to_file, const bstring& root_tag, const exportable& object);
ORK_ORK_EXT(void)
load_and_parse(bi_stream& fin, pugi::xml_document& root); // Just create a file with error checking
ORK_ORK_EXT(void)
load_and_parse_permissive(const file::path& path_to_file, const bstring& root_tag, importable& object);


/*
These are permissive interfaces
*/
template<typename T>
void value_to_attribute(node& node, const bstring& tag, ORK_CPARAM_T value)
{
    try {
        node.append_attribute(tag.c_str()).set_value(to_bstring(value).c_str());
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to set attribute ") << ORK_BYTE_2_STR(node.name())
            << ORK(":") << ORK_BYTE_2_STR(tag) << ORK(", ") << e.what();
    }
    catch(...) {
        ORK_LOG(severity_level::error)
            << ORK("Something happened setting attribute ")
            << ORK_BYTE_2_STR(node.name()) << ORK(":") << ORK_BYTE_2_STR(tag);
    }
}
template<typename T>
void value_to_xml(node& node, const bstring& tag, ORK_CPARAM_T value)
{
    try {
        node.append_child(tag.c_str())
            .append_child(pugi::node_pcdata)
            .set_value(to_bstring(value).c_str());
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to set tag ") << ORK_BYTE_2_STR(tag) << ORK(": ")
            << ORK_BYTE_2_STR(e.what());
    }
    catch(...) {
        ORK_LOG(severity_level::error)
            << ORK("Something happened setting tag ") << ORK_BYTE_2_STR(tag);
    }
}
template<typename T>
void value_to_xml(node& node, ORK_CPARAM_T value)
{
    try {
        node.append_child(pugi::node_pcdata).set_value(to_bstring(value).c_str());
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


template<typename T>
void value_from_attribute(const node& node, const bstring& tag, ORK_REF_T value)
{
    try {
        value = from_string<ORK_VAL_T>(node.attribute(tag.c_str()).value());
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to read parameter ") << ORK_BYTE_2_STR(tag)
            << ORK(": ") << ORK_BYTE_2_STR(e.what());
    }
    catch(...) {
        ORK_LOG(severity_level::error)
            << ORK("Something happened reading parameter ") << ORK_BYTE_2_STR(tag);
    }
}
template<typename T>
void value_from_xml(const node& node, const bstring& tag, ORK_REF_T value)
{
    try {
        value = from_string<ORK_VAL_T>(node.child_value(tag.c_str()));
    }
    catch(std::exception& e) {
        ORK_LOG(severity_level::error)
            << ORK("Failed to read tag ") << ORK_BYTE_2_STR(tag) << ORK(": ")
            << ORK_BYTE_2_STR(e.what());
    }
    catch(...) {
        ORK_LOG(severity_level::error)
            << ORK("Something happened reading tag ") << ORK_BYTE_2_STR(tag);
    }
}
template<typename T>
void value_from_xml(const node& node, ORK_REF_T value)
{
    try {
        value = from_string<ORK_VAL_T>(node.child_value());
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


// Overloads to eliminate dependency definitions
#    define ORK_XML_SERIALIZE_DECL_(API, TYPE) \
        API(void) \
        to_xml(ork::xml::node& node, const ork::bstring& tag, ORK_CPARAM(TYPE) value); \
        API(void) \
        to_attribute( \
            ork::xml::node& node, const ork::bstring& tag, ORK_CPARAM(TYPE) value); \
        API(void) \
        from_xml(const ork::xml::node& node, const ork::bstring& tag, ORK_REF(TYPE) value); \
        API(void) \
        from_attribute( \
            const ork::xml::node& node, const ork::bstring& tag, ORK_REF(TYPE) value)


#    define ORK_XML_SERIALIZE_DECL(TYPE) ORK_XML_SERIALIZE_DECL_(ORK_EXT, TYPE)
#    define ORK_ORK_XML_SERIALIZE_DECL(TYPE) \
        ORK_XML_SERIALIZE_DECL_(ORK_ORK_EXT, TYPE)

ORK_ORK_XML_SERIALIZE_DECL(int);
ORK_ORK_XML_SERIALIZE_DECL(unsigned);
ORK_ORK_XML_SERIALIZE_DECL(size_t);
ORK_ORK_XML_SERIALIZE_DECL(float);
ORK_ORK_XML_SERIALIZE_DECL(double);
ORK_ORK_XML_SERIALIZE_DECL(bstring);
ORK_ORK_XML_SERIALIZE_DECL(wstring);

#    define ORK_XML_SERIALIZE_DEF(TYPE) \
        void to_xml(node& node, const ork::bstring& tag, ORK_CPARAM(TYPE) value) \
        { \
            ork::xml::value_to_xml<TYPE>(node, tag, value); \
        } \
        void to_attribute(node& node, const ork::bstring& tag, ORK_CPARAM(TYPE) value) \
        { \
            ork::xml::value_to_attribute<TYPE>(node, tag, value); \
        } \
        void from_xml(const node& node, const ork::bstring& tag, ORK_REF(TYPE) value) \
        { \
            ork::xml::value_from_xml<TYPE>(node, tag, value); \
        } \
        void from_attribute( \
            const node& node, const ork::bstring& tag, ORK_REF(TYPE) value) \
        { \
            ork::xml::value_from_xml<TYPE>(node, tag, value); \
        }


#endif // ORK_USE_PUGI


} // namespace xml
namespace yaml {


#if ORK_USE_YAML


using node = YAML::Node;
ORK_OI_DECL_(yaml)


ORK_ORK_EXT(void)
export_file(const string& path_to_file, const node& root);
ORK_ORK_EXT(void)
export_file(const string& path_to_file, const exportable& object);
ORK_ORK_EXT(void) load_and_parse(bi_stream& fin, node& root);
ORK_ORK_EXT(node) load_and_parse(bi_stream& fin);


#endif // ORK_USE_YAML


} // namespace yaml
} // namespace ork
