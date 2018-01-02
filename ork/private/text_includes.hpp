/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"

#if ORK_USE_JSON
#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4668) //Json undefined macro
#endif
#include"json/reader.h"
#include"json/value.h"
#include"json/writer.h"
#if ORK_MSC
#pragma warning(pop)
#endif
#endif

#if ORK_USE_PUGI
#include"pugixml.hpp"
#endif

#if ORK_USE_YAML
#define YAML_CPP_DLL 1//We compensate for lack of configuration header
#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4127) //conditional expression is constant
#pragma warning(disable:4251) //needs to have dll-interface
#pragma warning(disable:4365) //signed/unsigned mismatch
#pragma warning(disable:4571) //catch(...) semantics changed
#pragma warning(disable:4625) //copy constructor was implicitly deleted
#pragma warning(disable:4626) //copy assigment was implicitly deleted
#pragma warning(disable:4668) //undefined preprocessor macro
#pragma warning(disable:5026) //move constructor implicitly deleted
#pragma warning(disable:5027) //move assignment implicitly deleted
#endif
#include"yaml-cpp/yaml.h"
#if ORK_MSC
#pragma warning(pop)
#endif
#endif
