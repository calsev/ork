/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/glm.hpp"
#include"ork/text_io.hpp"


namespace ork {


#if ORK_USE_GLM


class vector
#if ORK_USE_JSON || ORK_USE_PUGI || ORK_USE_YAML
	: public
#endif
#if ORK_USE_JSON
	json::exportable
#endif
#if ORK_USE_JSON && (ORK_USE_PUGI || ORK_USE_YAML)
	,
#endif
#if ORK_USE_PUGI
	xml::exportable
#endif
#if ORK_USE_PUGI && ORK_USE_YAML
	,
#endif
#if ORK_USE_YAML
	yaml::exportable
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
#if ORK_USE_JSON
	ORK_ORK_API explicit vector(Json::Value &node);
#endif
#if ORK_USE_PUGI
	ORK_ORK_API explicit vector(pugi::xml_node &node);
#endif
#if ORK_USE_YAML
	ORK_ORK_API explicit vector(YAML::Node &node);
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
#if ORK_USE_JSON
	ORK_ORK_API virtual void export_json(Json::Value &n) const;
#endif
#if ORK_USE_PUGI
	ORK_ORK_API virtual void export_xml(pugi::xml_node &n) const;
#endif
#if ORK_USE_YAML
	ORK_ORK_API virtual void export_yaml(YAML::Node &n) const;
#endif
};


ORK_ORK_API o_stream &operator << (o_stream&stream, const ork::vector &vec);


#endif//ORK_USE_GLM


}//namespace ork
