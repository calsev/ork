/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"

#if ORK_USE_GLM
#    include "glm/fwd.hpp"
#endif // ORK_USE_GLM

namespace ork {


/*
From color.hpp
*/
enum class color_space;
#if ORK_USE_GLM
using color4 = glm::vec4;
#endif // ORK_USE_GLM

/*
From command_line.hpp
*/
#if ORK_USE_BOOST
class command_handler;
#endif // ORK_USE_BOOST

/*
From distribution.hpp
*/
#if ORK_USE_BOOST
class command_handler;
class random;
#endif // ORK_USE_BOOST
template<typename T>
class triangle_distribution;
template<typename T>
class trapezoid_distribution;

/*
From file_utils.hpp
*/
template<class functor, class iter_t, class sort>
struct directory_executer;
template<class functor, class search_type, class sort_type>
struct iterate_directory;
template<class T>
struct directory_range;

/*
From filter.hpp
*/
#if ORK_USE_BOOST
template<unsigned C>
struct ring;
template<unsigned order, unsigned inverse_alpha>
struct butterworth;
#endif // ORK_USE_BOOST

/*
From geometry.hpp
*/
enum class angle;
template<angle>
struct circle;

namespace GLM {

class bounding_box;
class interval;
struct segment;
class chain;

namespace MC {
struct view;
struct rotated_view;
} // namespace MC

} // namespace GLM

/*
From glm.hpp
*/
namespace GLM {

struct dunit3;
template<typename T>
struct default_epsilon_factor;

} // namespace GLM

/*
From html.hpp
*/
enum class align;
enum class border;

namespace html {

struct exportable;
struct pair;
struct string;
struct heading;
struct page_break;
struct padding;
struct image;
struct style;
struct image_style;
struct div_style;
struct table_style;
struct line_style;
struct style_set;
struct header;
struct line;
struct label;
struct table_element;
struct table;
struct div;
struct body;
struct document;

} // namespace html


/*
From memory.hpp
*/
template<typename T>
struct default_deleter;
template<typename T>
struct singleton_deleter;
template<class T, class D>
class value_ptr;
template<typename T, typename D>
class shared_ptr;

} // namespace ork
