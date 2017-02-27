/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<algorithm>
#include<iomanip>
#include"boost/algorithm/string.hpp"

#include"ork/color.hpp"
#include"ork/distribution.hpp"

#if ORK_USE_GLM
#include"glm/vec3.hpp"

namespace ork {


#if ORK_DEBUG
#define ORK_NEG_COLOR_CHECK(COLOR)\
if(COLOR.r < 0.f || COLOR.g < 0.f || COLOR.b < 0.f) {\
	ORK_THROW(ORK("Negative color"));\
}
#else
#define ORK_NEG_COLOR_CHECK(COLOR)
#endif


//Waiting for C++17
ORK_INLINE float clamp(const float val, const float min, const float max) {
	return std::min(std::max(val, min), max);
}


float to_hex_space(const color4&c, const float max, const float chroma) {
	/*
	Six vertices for hexagon
	Define periodic, circular space: red is 0, green is 2, blue is 4
	*/
	if(chroma <= 0.f) {
		return 0.f;//By convention, to avoid divide by 0;
	}
	else if(max <= c.r) {//The red third, rotate [+1, -1] about datum 0;
		const float hue = (c.g - c.b) / chroma;
		return hue >= 0.f ? hue : hue + 6.f;//Perdiodicity, stay in [0, 1]
	}
	else if(max <= c.g) {//The green third, rotate [+1, -1] about datum 2;
		return (c.b - c.r) / chroma + 2.f;
	}
	else {//max <= c.b //The blue third, rotate [+1, -1] about datum 4;
		return (c.r - c.g) / chroma + 4.f;
	}
}


float value_or_lightness(const float min, const float max, const color_space cs) {
	switch(cs) {
	case color_space::hsv:
		return max;
	case color_space::hsl:
		return (max + min)*0.5f;
	default:
		ORK_THROW(ORK("Invalid color space"));
	}
}
float calc_saturation(const float chroma, const float value_or_lightness, const color_space cs) {
	switch(cs) {
	case color_space::hsv:
		if(value_or_lightness <= 0.f) {
			return 0.f;
		}
		return chroma / value_or_lightness;//(max - min)/max
	case color_space::hsl:
		if(value_or_lightness <= 0.f || 1.f <= value_or_lightness) {
			return 0.f;
		}
		return chroma / (1.f - std::abs(2.f * value_or_lightness - 1.f));//(max - min)/(1 - |(max + min) - 1|)
	default:
		ORK_THROW(ORK("Invalid color space"));
	}
}


glm::vec3 calc_rgb(const float C, const float min, const float h_6) {
	const float X = C*(1.f - std::abs(std::fmod(h_6, 2.f) - 1.f));
	if(h_6 < 1.f) {
		return glm::vec3{C, X, 0.f}+min;
	}
	else if(h_6 < 2.f) {
		return glm::vec3{X, C, 0.f}+min;
	}
	else if(h_6 < 3.f) {
		return glm::vec3{0.f, C, X}+min;
	}
	else if(h_6 < 4.f) {
		return glm::vec3{0.f, X, C}+min;
	}
	else if(h_6 < 5.f) {
		return glm::vec3{X, 0.f, C}+min;
	}
	else {
		return glm::vec3{C, 0.f, X}+min;
	}
}


color4 truncate_hue(const color4&c, const color_space cs) {
	if(cs == color_space::rgb) {
		return c;//This call does not really make sense, but we allow it
	}
	color4 retval(c);
	if(std::min(c.g, c.b) <= 0.f) {//Saturation or bottom of cone, hue = 0 by convention
		retval.r = 0.f;
	}
	if(cs == color_space::hsl && c.b >= 1.0) {//Top of cone
		retval.r = 0.f;
	}
	if(retval.r >= 1.f) {
		retval.r -= 1.f;//Origin wraps to 0
	}
	return retval;
}


color4 truncate_saturation(const color4&c, const color_space cs) {
	if(cs == color_space::rgb) {
		return c;//This call does not really make sense, but we allow it
	}
	color4 retval(c);
	if(c.b <= 0.f) {//Saturation or bottom of cone, hue = 0 by convention
		retval.g = 0.f;
	}
	if(cs == color_space::hsl && c.b >= 1.0) {//Top of cone
		retval.g = 0.f;
	}
	return retval;
}


color4 truncate(const color4&c, const color_space cs) {
	return truncate_hue(truncate_saturation(c, cs), cs);
}


color4 convert(const color4&c, const color_space from_space, const color_space to_space) {
	/*
	RGB cube, HSV cylinder, HSL cone
	In short, turn the cube so 0,0,0 is at bottom and 1,1,1 is at top.
	The other 6 points of the cube project onto the corners of a hexagon.
	Rotate so that red is at 0 datum.
	See https://en.wikipedia.org/wiki/HSL_and_HSV for a good geometric description.
	*/
	if(from_space == to_space) {//A useful invariant below
		return c;
	}
	if(from_space == color_space::rgb) {
		const float min = std::min(c.r, std::min(c.g, c.b));
		const float max = std::max(c.r, std::max(c.g, c.b));
		const float chroma = max - min;//The normalized scale for color magnitudes

		const float hue = to_hex_space(c, max, chroma) / 6.f;//Normalize six-point space to unit interval
		const float val_light = value_or_lightness(min, max, to_space);
		const float saturation = calc_saturation(chroma, val_light, to_space);

		color4 retval{hue, saturation, val_light, c.a};//Debugging
		ORK_NEG_COLOR_CHECK(retval);
		return retval;
	}
	else {
		const bool is_hsv = from_space == color_space::hsv;//Otherwise HSL

		const float chroma = is_hsv
			? c.y*c.z//S*V
			: (1.f - std::abs(2.f*c.z - 1.f))*c.y//(1 - |2*L - 1|)*S
			;
		const float min = std::max(0.f, is_hsv
			? c.z - chroma
			: c.z - chroma*0.5f
		);
		if(to_space == color_space::rgb) {
			const float h_6 = c.x*6.f;//To hex coordinates
			const glm::vec3 rgb{calc_rgb(chroma, min, h_6)};
			color4 retval{rgb, c.a};//Debugging
			ORK_NEG_COLOR_CHECK(retval);
			return retval;
		}
		else {
			const float max = is_hsv
				? c.z
				: c.z + chroma*0.5f
				;
			const float val_light = value_or_lightness(min, max, to_space);
			const float saturation = calc_saturation(chroma, val_light, to_space);
			color4 retval{c.x, saturation, val_light, c.a};//Debugging
			ORK_NEG_COLOR_CHECK(retval);
			return retval;
		}
	}
	
}


ORK_ORK_API const color4 red = {1., 0., 0., 1.};
ORK_ORK_API const color4 green = {0., 1., 0., 1.};
ORK_ORK_API const color4 blue = {0., 0., 1., 1.};
ORK_ORK_API const color4 white = {1., 1., 1., 1.};


color4 normalized_lightness(const color4&c, const float lightness, const color_space cs) {
	color4 hsl = convert(c, cs, color_space::hsl);
	hsl.b = lightness;
	return convert(hsl, color_space::hsl, cs);
}
float luma(const color4&rgb) {
#if 0//luma 601 goes overboard; blue is clearly brightest
	return 0.30f*c.r + 0.59f*c.g + 0.11f*c.b;
#elif 0//This was a little dark in the red-green range
	return 0.3f*rgb.r + 0.5f*rgb.g + 0.2f*rgb.b;
#else
	return 0.32f*rgb.r + 0.46f*rgb.g + 0.22f*rgb.b;
#endif
}
color4 normalized_luma(const color4&c, const float lum, const color_space cs) {
	/*
	This is a simplified model using luma 601 coefficients https://en.wikipedia.org/wiki/HSL_and_HSV.
	Basically, yellow-green appears light and violet-blue appears dark.
	*/
	color4 rgb(convert(c, cs, color_space::rgb));
	if(rgb.r <= 0.f && rgb.g <= 0.f && rgb.b <= 0.f) {
		return convert(white*lum, color_space::rgb, cs);
	}
	float curr_luma = luma(rgb);
	while(std::abs(lum - curr_luma) > 0.02) {//Arbitrary magic number
		color4 hsl(convert(rgb, color_space::rgb, color_space::hsl));
		hsl.b = std::min(1.f, hsl.b*(1.f + 0.2f*(lum - curr_luma)));
		rgb = convert(hsl, color_space::hsl, color_space::rgb);
		curr_luma = luma(rgb);
	}
	return convert(rgb, color_space::rgb, cs);
}


color4 normalized_hue(const float value) {//Value is defined on [0, 1]
	/*
	This is waay simplified, and uses made-up triangle distributions.
	If we offset peaks by 0.05 green hues are too close.
	If we offset peaks by 0.10 red and blue hues are too close.
	So we offset by 0.05 and place a dead zone of 0.10 centered around pure green.
	*/
	static ork::triangle_distribution<float>blue_l{-0.95f, -0.5f, 0.05f};//Periodicity
	static ork::triangle_distribution<float>red_l{-0.5f, 0.05f, 0.5f};//0.0 should be pure red
	static ork::triangle_distribution<float>green{0.05f, 0.5f, 0.95f};//0.5 is pure green
	static ork::triangle_distribution<float>blue_h{0.5f, 0.95f, 1.5f};//1.0 should be pure blue
	static ork::triangle_distribution<float>red_h{0.95f, 1.5f, 1.95f};//Periodicity

	const bool bottom_half = value < 0.5;
	//half-range of value is 0.5, half-range of dead-zone scale is 0.55/2 + (0.45 - 0.05) = 0.275 + 0.4 = 0.675
	const float scaled = (bottom_half ? value : value - 0.5f)*0.675f / 0.5f;

	//Bottom start point is 0.05 - 0.55/2 = 0.05 - 0.275 = -0.225
	//Top start point is 0.5 + 0.05 = 0.55
	float transformed = bottom_half ? -0.225 + scaled : 0.55 + scaled;
	const float rl = red_l(transformed);
	const float rh = red_h(transformed);
	const float g = green(transformed);
	const float bl = blue_l(transformed);
	const float bh = blue_h(transformed);
	const glm::dvec4 normed{rl + rh, g, bl + bh, 1.f};
	return normed;
}


color4 normalized_red_green(const float weight) {
	const float alpha = std::pow(std::max(0.f, std::min(1.f, weight)), 1.3f);
	return (1.f - alpha)*red + alpha*green;
}


std::vector<color4> contrast_array(const size_t size) {
	return contrast_array(size, 0.2f);
}
std::vector<color4>contrast_array(const size_t size, const float luma) {
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
	std::vector<color4>retval;
	LOOPI(size) {
		const float val = float(i) / float(size);//Size bars separate size + 1 spaces
		const color4 hue(normalized_hue(val));
		const color4 luma(normalized_luma(hue, luma, color_space::rgb));
		retval.push_back(luma);
	}
	return std::move(retval);
}


string to_hex(const color4&c, const color_space cs) {
	const color4 rgb(convert(c, cs, color_space::rgb));
	ork::o_string_stream stream;
	stream	<< std::setfill(ORK('0')) << std::hex
			<< std::setw(2) << short(rgb.r*255.f) << std::setw(2) << short(rgb.g*255.f) << std::setw(2) << short(rgb.b*255.f) << std::setw(2) << short(rgb.a*255.f);
	return boost::to_upper_copy(stream.str());
}


}//namespace ork

namespace glm {

ork::o_stream&operator<<(ork::o_stream&stream, const ork::color4&c) {
	stream << ORK("(") << c.r << ORK(", ") << c.g << ORK(", ") << c.b << ORK(", ") << c.a << ORK(")");
	return stream;
}

}

#endif//ORK_USE_GLM
