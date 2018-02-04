/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/ork.hpp"
#define ORK_STL_INC_FILE <algorithm>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <iomanip>
#include "ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include "ork/core/stl_include.inl"

#include "ork/color.hpp"
#include "ork/distribution.hpp"
#include "ork/string_utils.hpp"


#if ORK_USE_GLM

#    include "glm/glm.hpp"
#    include "glm/vec3.hpp"
#    include "glm/gtx/compatibility.hpp"


namespace ork {


#    if ORK_DEBUG
#        define ORK_NEG_COLOR_CHECK(COLOR) \
            if(COLOR.r < 0.f || COLOR.g < 0.f || COLOR.b < 0.f) { \
                ORK_THROW(ORK("Negative color")); \
            }
#    else
#        define ORK_NEG_COLOR_CHECK(COLOR)
#    endif


// Waiting for C++17
ORK_INLINE float clamp(const float val, const float min, const float max)
{
    return std::min(std::max(val, min), max);
}


float to_hex_space(const color4& c, const float max, const float chroma)
{
    /*
    Six vertices for hexagon
    Define periodic, circular space: red is 0, green is 2, blue is 4
    */
    if(chroma <= 0.f) {
        return 0.f; // By convention, to avoid divide by 0;
    }
    else if(max <= c.r) { // The red third, rotate [+1, -1] about datum 0;
        const float hue = (c.g - c.b) / chroma;
        return hue >= 0.f ? hue : hue + 6.f; // Perdiodicity, stay in [0, 1]
    }
    else if(max <= c.g) { // The green third, rotate [+1, -1] about datum 2;
        return (c.b - c.r) / chroma + 2.f;
    }
    else { // max <= c.b //The blue third, rotate [+1, -1] about datum 4;
        return (c.r - c.g) / chroma + 4.f;
    }
}


float value_or_lightness(const float min, const float max, const color_space cs)
{
    switch(cs) {
        case color_space::hsv:
            return max;
        case color_space::hsl:
            return (max + min) * 0.5f;
        case color_space::rgb:
        default:
            ORK_THROW(ORK("Invalid color space"));
    }
}
float calc_saturation(const float chroma, const float value_or_lightness, const color_space cs)
{
    switch(cs) {
        case color_space::hsv:
            if(value_or_lightness <= 0.f) {
                return 0.f;
            }
            return chroma / value_or_lightness; //(max - min)/max
        case color_space::hsl:
            if(value_or_lightness <= 0.f || 1.f <= value_or_lightness) {
                return 0.f;
            }
            return chroma / (1.f - std::abs(2.f * value_or_lightness - 1.f)); //(max - min)/(1 - |(max + min) - 1|)
        case color_space::rgb:
        default:
            ORK_THROW(ORK("Invalid color space"));
    }
}


glm::vec3 calc_rgb(const float C, const float min, const float h_6)
{
    const float X = C * (1.f - std::abs(std::fmod(h_6, 2.f) - 1.f));
    if(h_6 < 1.f) {
        return glm::vec3{C, X, 0.f} + min;
    }
    else if(h_6 < 2.f) {
        return glm::vec3{X, C, 0.f} + min;
    }
    else if(h_6 < 3.f) {
        return glm::vec3{0.f, C, X} + min;
    }
    else if(h_6 < 4.f) {
        return glm::vec3{0.f, X, C} + min;
    }
    else if(h_6 < 5.f) {
        return glm::vec3{X, 0.f, C} + min;
    }
    else {
        return glm::vec3{C, 0.f, X} + min;
    }
}


color4 truncate_hue(const color4& c, const color_space cs)
{
    if(cs == color_space::rgb) {
        return c; // This call does not really make sense, but we allow it
    }
    color4 retval(c);
    if(std::min(c.g, c.b) <= 0.f) { // Saturation or bottom of cone, hue = 0 by convention
        retval.r = 0.f;
    }
    if(cs == color_space::hsl && c.b >= 1.0) { // Top of cone
        retval.r = 0.f;
    }
    if(retval.r >= 1.f) {
        retval.r -= 1.f; // Origin wraps to 0
    }
    return retval;
}


color4 truncate_saturation(const color4& c, const color_space cs)
{
    if(cs == color_space::rgb) {
        return c; // This call does not really make sense, but we allow it
    }
    color4 retval(c);
    if(c.b <= 0.f) { // Saturation or bottom of cone, hue = 0 by convention
        retval.g = 0.f;
    }
    if(cs == color_space::hsl && c.b >= 1.0) { // Top of cone
        retval.g = 0.f;
    }
    return retval;
}


color4 truncate(const color4& c, const color_space cs)
{
    return truncate_hue(truncate_saturation(c, cs), cs);
}


color4 convert(const color4& c, const color_space from_space, const color_space to_space)
{
    /*
    RGB cube, HSV cylinder, HSL cone
    In short, turn the cube so 0,0,0 is at bottom and 1,1,1 is at top.
    The other 6 points of the cube project onto the corners of a hexagon.
    Rotate so that red is at 0 datum.
    See https://en.wikipedia.org/wiki/HSL_and_HSV for a good geometric description.
    */
    if(from_space == to_space) { // A useful invariant below
        return c;
    }
    if(from_space == color_space::rgb) {
        const float min = std::min(c.r, std::min(c.g, c.b));
        const float max = std::max(c.r, std::max(c.g, c.b));
        const float chroma = max - min; // The normalized scale for color magnitudes

        const float hue = to_hex_space(c, max, chroma) /
                          6.f; // Normalize six-point space to unit interval
        const float val_light = value_or_lightness(min, max, to_space);
        const float saturation = calc_saturation(chroma, val_light, to_space);

        color4 retval{hue, saturation, val_light, c.a}; // Debugging
        ORK_NEG_COLOR_CHECK(retval);
        return retval;
    }
    else {
        const bool is_hsv = from_space == color_space::hsv; // Otherwise HSL

        const float chroma = is_hsv ? c.y * c.z // S*V
                                    : (1.f - std::abs(2.f * c.z - 1.f)) * c.y //(1 - |2*L - 1|)*S
            ;
        const float min = std::max(0.f, is_hsv ? c.z - chroma : c.z - chroma * 0.5f);
        if(to_space == color_space::rgb) {
            const float h_6 = c.x * 6.f; // To hex coordinates
            const glm::vec3 rgb{calc_rgb(chroma, min, h_6)};
            color4 retval{rgb, c.a}; // Debugging
            ORK_NEG_COLOR_CHECK(retval);
            return retval;
        }
        else {
            const float max = is_hsv ? c.z : c.z + chroma * 0.5f;
            const float val_light = value_or_lightness(min, max, to_space);
            const float saturation = calc_saturation(chroma, val_light, to_space);
            color4 retval{c.x, saturation, val_light, c.a}; // Debugging
            ORK_NEG_COLOR_CHECK(retval);
            return retval;
        }
    }
}


namespace rgb {
const color4 red = {1.0, 0.0, 0.0, 1.0};
const color4 yellow = {1.0, 1.0, 0.0, 1.0};
const color4 green = {0.0, 1.0, 0.0, 1.0};
const color4 cyan = {0.0, 1.0, 1.0, 1.0};
const color4 blue = {0.0, 0.0, 1.0, 1.0};
const color4 magenta = {1.0, 0.0, 1.0, 1.0};
const color4 white = {1.0, 1.0, 1.0, 1.0};
const color4 black = {0.0, 0.0, 0.0, 1.0};
} // namespace rgb
namespace hsv {
const color4 red = {0.0, 1.0, 1.0, 1.0};
const color4 yellow = {0.5 / 3.0, 1.0, 1.0, 1.0};
const color4 green = {1.0 / 3.0, 1.0, 1.0, 1.0};
const color4 cyan = {1.5 / 3.0, 1.0, 1.0, 1.0};
const color4 blue = {2.0 / 3.0, 1.0, 1.0, 1.0};
const color4 magenta = {2.5 / 3.0, 1.0, 1.0, 1.0};
const color4 white = {0.0, 0.0, 1.0, 1.0};
const color4 black = {0.0, 0.0, 0.0, 1.0};
} // namespace hsv
namespace hsl {
const color4 red = {0.0, 1.0, 0.5, 1.0};
const color4 yellow = {0.5 / 3.0, 1.0, 0.5, 1.0};
const color4 green = {1.0 / 3.0, 1.0, 0.5, 1.0};
const color4 cyan = {1.5 / 3.0, 1.0, 0.5, 1.0};
const color4 blue = {2.0 / 3.0, 1.0, 0.5, 1.0};
const color4 magenta = {2.5 / 3.0, 1.0, 0.5, 1.0};
const color4 white = {0.0, 0.0, 1.0, 1.0};
const color4 black = {0.0, 0.0, 0.0, 1.0};
} // namespace hsl

#    define ORK_COLOR_FUNC(COLOR) \
        const color4& COLOR(const color_space cs) \
        { \
            switch(cs) { \
                case color_space::rgb: \
                    return rgb::COLOR; \
                case color_space::hsv: \
                    return hsv::COLOR; \
                case color_space::hsl: \
                    return hsl::COLOR; \
            } \
            ORK_UNREACHABLE \
        }
ORK_COLOR_FUNC(red)
ORK_COLOR_FUNC(yellow)
ORK_COLOR_FUNC(green)
ORK_COLOR_FUNC(cyan)
ORK_COLOR_FUNC(blue)
ORK_COLOR_FUNC(magenta)
ORK_COLOR_FUNC(white)
ORK_COLOR_FUNC(black)


color4 normalized_lightness(const color4& c, const float lightness, const color_space cs)
{
    color4 hsl = convert(c, cs, color_space::hsl);
    hsl.b = lightness;
    return convert(hsl, color_space::hsl, cs);
}


#    if 0 // luma 601 goes overboard; blue is clearly brightest
const glm::vec3 intensity = {0.30f, 0.59f, 0.11f};
#    elif 0 // This was a little dark in the red-green range
const glm::vec3 intensity = {0.30f, 0.50f, 0.20f};
#    elif 0 // Green was a bit bright and blue a bit dark
const glm::vec3 intensity = {0.32f, 0.46f, 0.22f};
#    else
const glm::vec3 intensity = {0.32f, 0.48f, 0.20f};
#    endif


float luma(const color4& rgb)
{
    return glm::dot(intensity, glm::vec3(rgb));
}


color4 normalized_luma(const color4& c, const float lum, const color_space cs)
{
    /*
    This is a simplified model using luma coefficients https://en.wikipedia.org/wiki/HSL_and_HSV.
    Basically, yellow-green appears light and violet-blue appears dark.
    */
    color4 rgb(convert(c, cs, color_space::rgb));
    if(rgb.r <= 0.f && rgb.g <= 0.f && rgb.b <= 0.f) {
        return white(cs) * lum;
    }
    float curr_luma = luma(rgb);
    while(std::abs(lum - curr_luma) > 0.02) { // Arbitrary magic number
        color4 hsl(convert(rgb, color_space::rgb, color_space::hsl));
        hsl.b = std::min(1.f, hsl.b * (1.f + 0.2f * (lum - curr_luma)));
        rgb = convert(hsl, color_space::hsl, color_space::rgb);
        curr_luma = luma(rgb);
    }
    return convert(rgb, color_space::rgb, cs);
}


float normalized_hue(const float hue)
{ // hue is defined on [0, 1]
    /*
    Human perception of color difference is not linear.
    We support a simple model of dead zones around primary colors and peak shifts.
    */
    // These are half-widths of dead zones
    static const float red_gap = 0.05f;
    static const float green_gap = 0.05f;
    static const float blue_gap = 0.10f;

    static const float green_peak = 1.7f; // 2.0 is nominal, compress red-green range
    static const float blue_peak = 3.85f; // 4.0 is nominal, center on remainder

    const float scale = 6.f;
    const float scaled_hue = hue * scale; // red-red is 1.5 period
    if(scaled_hue < green_peak) {
        static const float third_width = green_peak - 0.f;
        static const float third_scale = (2.f - red_gap - green_gap) / third_width;
        const float offset = 0.f + red_gap + third_scale * (scaled_hue - 0.f);
        return offset / scale;
    }
    else if(scaled_hue < blue_peak) {
        static const float third_width = blue_peak - green_peak;
        static const float third_scale = (2.f - green_gap - blue_gap) / third_width;
        const float offset = 2.f + green_gap + third_scale * (scaled_hue - green_peak);
        return offset / scale;
    }
    else {
        static const float third_width = 6.f - blue_peak;
        static const float third_scale = (2.f - blue_gap - red_gap) / third_width;
        const float offset = 4.f + blue_gap + third_scale * (scaled_hue - blue_peak);
        return offset / scale;
    }
}


color4 normalized_red_green(const float weight)
{
    const float alpha = std::pow(std::max(0.f, std::min(1.f, weight)), 1.3f);
    return (1.f - alpha) * red(color_space::rgb) + alpha * green(color_space::rgb);
}


std::vector<color4> contrast_array(const size_t size)
{
    return contrast_array(size, 0.2f);
}
std::vector<color4> contrast_array(const size_t size, const float luma)
{
    /*
    1.
    We want max difference between colors, garish is encouraged, so saturation is constant 100%.
    As a bonus there will be no grey that might be confused with the 'base' part.
    We are down to two dimensions.
    2.
    Colors with equal contrast with the background are percieved with equal importance.
    We want equal importance and assume a white backgrond.
    If only this meant constant lightness the problem would be reduced to one dimension, hue.
    3.
    Colors in proximity are perceived as more different.
    Humans process difference more easily bwtween colors in close proximity if variation is smallish.
    We can't account for spatial proximity within this function interface, but it is something to think about.
    */
    static const float luma_alpha = 0.90f; // Consecutive colors have luma toggled
    const float max_luma = std::min(
        1.f, 2.f * luma); // Blending to white can be too extreme when luma is dark
    const float top_luma = glm::lerp(max_luma, luma, luma_alpha);
    const float bottom_luma = luma * luma_alpha;

    std::vector<color4> retval;
    LOOPI(size)
    {
        const float val = float(i) / float(size); // Size bars separate size + 1 spaces
        const color4 hsl(normalized_hue(val), 1.f, luma, 1.f);
        const color4 luma_new(normalized_luma(
            hsl, i & 0x1 ? bottom_luma : top_luma, color_space::hsl));
        const color4 rgb(convert(luma_new, color_space::hsl, color_space::rgb));
        retval.push_back(rgb);
    }
    return std::move(retval);
}
std::vector<color4> grey_array(const size_t size, float min_luma, float max_luma)
{
    if(size == 1) {
        return std::vector<color4>{glm::lerp(
            black(color_space::rgb), white(color_space::rgb), (min_luma + max_luma) / 2.f)};
    }

    const color4 dark(
        glm::lerp(black(color_space::rgb), white(color_space::rgb), min_luma));
    const color4 light(
        glm::lerp(black(color_space::rgb), white(color_space::rgb), max_luma));

    std::vector<color4> retval;
    LOOPI(size)
    {
        const float alpha = float(i) / float(size - 1);
        retval.push_back(glm::lerp(dark, light, alpha));
    }
    return std::move(retval);
}


short round_hex(const float val)
{
    return static_cast<short>(val * 255.0 + 0.5);
}


string to_hex(const color4& c, const color_space cs)
{
    const color4 rgb(convert(c, cs, color_space::rgb));
    ork::o_string_stream stream;
    stream << std::setfill(ORK('0')) << std::hex << std::setw(2)
           << round_hex(rgb.r) << std::setw(2) << round_hex(rgb.g) << std::setw(2)
           << round_hex(rgb.b) << std::setw(2) << round_hex(rgb.a);
    return to_upper_copy(stream.str());
}
color4 from_hex(const string& c, const color_space cs)
{
    color4 retval{1.f};
    const size_t limit_i = c.size() < 8 ? static_cast<size_t>(3)
                                        : static_cast<size_t>(4);
    LOOPI(limit_i)
    {
        retval[static_cast<color4::length_type>(i)] = static_cast<float>(
            std::stoul(c.substr(i * 2, 2), nullptr, 16) / 255.0);
    }
    return convert(retval, color_space::rgb, cs);
}


} // namespace ork

namespace glm {

ork::o_stream& operator<<(ork::o_stream& stream, const ork::color4& c)
{
    stream << ORK("(") << c.r << ORK(", ") << c.g << ORK(", ") << c.b
           << ORK(", ") << c.a << ORK(")");
    return stream;
}

} // namespace glm

#endif // ORK_USE_GLM
