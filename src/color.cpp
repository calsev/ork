/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/color.hpp"
#include"ork/distribution.hpp"

#include"glm/vec3.hpp"

namespace ork {


const color4 red = {1., 0., 0., 1.};
const color4 green = {0., 1., 0., 1.};
const color4 blue = {0., 0., 1., 1.};


color4 normalized_lightness(const color4&c) {
	/*
	This is waay simplified, and uses made-up linear coefficients.
	Basically, yellow-green appears light and violet-blue appears dark.
	*/
	const float value = c.r*1.0f + c.g*1.3f + c.b*0.7f;
	return color4(glm::vec3(c / value), c.a);//Scale colors, keep alpha
}


color4 normalized_hue(const float value) {//Value is defined on [0, 1]
	/*
	This is waay simplified, and uses made-up triangle distributions.
	We multiply value by 1.5 instead of using thirds.
	Each color should be symmetrical and occupy 1.0 width.
	We narrow the green spectrum to 0.8.
	*/
	static ork::triangle_distribution<float>red_l{-0.5f, 0.1f, 0.5f};//0.0 should be pure red
	static ork::triangle_distribution<float>green{0.1f, 0.5f, 0.9f};//0.5 is pure green
	static ork::triangle_distribution<float>blue{0.5f, 0.9f, 1.5f};//1.0 should be pure blue
	static ork::triangle_distribution<float>red_h{0.9f, 1.5f, 2.1f};//Periodicity

	const float val = value*1.5f;
	return{red_l(val) + red_h(val), green(val), blue(val), 1.f};
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