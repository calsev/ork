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
const auto first_char_p = s3::alpha | s3::char_('_');
const auto last_char_p = s3::alnum | s3::char_('_');
const auto not_quote_p = s3::char_ - s3::char_('"');
const auto not_eol_p = s3::char_ - s3::eol;
const auto eol_delim_p = s3::lit('#') | s3::lit("//");
const auto eol_com_p = eol_delim_p >> not_eol_p;

auto const alpha_bool_def = true_p[set_true] | false_p[set_false];
ORK_PARSER_DEF(alpha_bool)

auto const id_def = first_char_p >> +last_char_p;
ORK_PARSER_DEF(id)

auto const name_def = first_char_p >> +(last_char_p | s3::char_("-'"));
ORK_PARSER_DEF(name)

auto const quote_def = s3::lit('"') >> s3::lexeme[*not_quote_p >> s3::lit('"')];
ORK_PARSER_DEF(quote)

auto const at_var_def = s3::lit('@') >> +last_char_p;
ORK_PARSER_DEF(at_var)

auto const com_skip_def = s3::lexeme[s3::omit[+(*s3::space >> -eol_com_p)]];
ORK_PARSER_DEF(com_skip)

} // namespace x3
} // namespace ork

#endif // ORK_USE_BOOST
