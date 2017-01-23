/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/color.hpp"
#include"ork/distribution.hpp"

#if ORK_USE_GLM
#include"glm/vec3.hpp"

namespace ork {


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
