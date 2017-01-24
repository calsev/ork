/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/color.hpp"
#include"ork/distribution.hpp"

#if ORK_USE_GLM
#include"glm/vec3.hpp"

namespace ork {


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

		return color4{hue, saturation, val_light, c.a};
	}
	else {
		const bool is_hsv = from_space == color_space::hsv;//Otherwise HSL

		const float chroma = is_hsv
			? c.y*c.z//S*V
			: (1.f - std::abs(2.f*c.z - 1.f))*c.y//(1 - |2*L - 1|)*S
			;

		const float h_6 = c.x*6.f;//To hex coordinates
		const float min = is_hsv
			? c.z - chroma
			: c.z - chroma*0.5f
			;
		const glm::vec3 rgb{calc_rgb(chroma, min, h_6)};
		return color4{rgb,c.a};
	}
	
}


ORK_ORK_API const color4 red = {1., 0., 0., 1.};
ORK_ORK_API const color4 green = {0., 1., 0., 1.};
ORK_ORK_API const color4 blue = {0., 0., 1., 1.};


color4 normalized_lightness(const color4&c) {
	return normalized_lightness(c, 1.f);
}
color4 normalized_lightness(const color4&c, const float one_norm) {
	/*
	This is waay simplified, and uses made-up linear coefficients.
	Basically, yellow-green appears light and violet-blue appears dark.
	*/
	const float value = c.r*1.0f + c.g*1.3f + c.b*0.7f;
	const glm::vec3 normed(c*one_norm / value);//For debugging
	return color4(normed, c.a);//Scale colors, keep alpha
}


color4 normalized_hue(const float value) {//Value is defined on [0, 1]
	/*
	This is waay simplified, and uses made-up triangle distributions.
	We multiply value by 1.5 instead of using thirds.
	Each color should be symmetrical and occupy 1.0 width.
	We narrow the green spectrum to 0.8.
	*/
	static ork::triangle_distribution<float>blue_l{-1.0f, -0.5f, 0.1f};//Periodicity
	static ork::triangle_distribution<float>red_l{-0.5f, 0.1f, 0.5f};//0.0 should be pure red
	static ork::triangle_distribution<float>green{0.1f, 0.5f, 0.9f};//0.5 is pure green
	static ork::triangle_distribution<float>blue_h{0.5f, 0.9f, 1.5f};//1.0 should be pure blue
	static ork::triangle_distribution<float>red_h{0.9f, 1.5f, 2.0f};//Periodicity

	const float rl = red_l(value);
	const float rh = red_h(value);
	const float g = green(value);
	const float bl = blue_l(value);
	const float bh = blue_h(value);
	const glm::dvec4 normed{rl + rh, g, bl + bh, 1.f};
	return normed;
}


color4 normalized_red_green(const float weight) {
	const float alpha = std::pow(std::max(0.f, std::min(1.f, weight)), 1.3f);
	return (1.f - alpha)*red + alpha*green;
}


std::vector<color4>contrast_array(const size_t size) {
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
		const float val = float(i) / float(size - 1);
		retval.push_back(normalized_lightness(normalized_hue(val)));
	}
	return std::move(retval);
}


}//namespace ork

namespace glm {

ork::o_stream&operator<<(ork::o_stream&stream, const ork::color4&c) {
	stream << ORK("(") << c.r << ORK(", ") << c.g << ORK(", ") << c.b << ORK(", ") << c.a << ORK(")");
	return stream;
}

}

#endif//ORK_USE_GLM
