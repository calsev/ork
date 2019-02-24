/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/parse.hpp"


#if ORK_USE_BOOST

namespace ork {
namespace x3 {

auto set_true = [&](auto& ctx) { _val(ctx) = true; };
auto set_false = [&](auto& ctx) { _val(ctx) = false; };

const auto true_p = s3::lit("true") | s3::lit("yes") | s3::lit("on");
const auto false_p = s3::lit("false") | s3::lit("no") | s3::lit("off");

auto const alpha_bool_def = true_p[set_true] | false_p[set_false];

ORK_PARSER_DEF(alpha_bool)

} // namespace x3
} // namespace ork

#endif // ORK_USE_BOOST
