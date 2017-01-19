/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include<vector>
#include"ork/ork.hpp"

#if ORK_USE_GLM
#include"glm/vec4.hpp"


namespace ork {


typedef glm::vec4 color4;


/*
Human color perception is not linear, see Munsell, L*c*h, and L*a*b color systems. https://en.wikipedia.org/wiki/Color_difference.
This affects perceived lightness as yellow with the same lightness is percieved as much brighter than violet.
Lightness perception is not even linear within a hue.
This also affects perceived contrast as green must have greater variation.

If someone wants to implement an advanced color space, please do.
To check this box with minimal effort:
a.
Yellows must be darker.
We use a weighted normalization that darkens green-yellow and lightens blues.
b.
The scale must favor greater variation in green.
We unroll a unit circle for hue, but with smaller variance for green distribution.
*/
ORK_ORK_EXT(color4) normalized_lightness(const color4&c);

ORK_ORK_EXT(color4) normalized_hue(const float value);//Value is defined on [0, 1]

ORK_ORK_EXT(color4) normalized_red_green(const float weight);//Returns a normalized color on continuum red-green weighted on interval [0, 1]

ORK_ORK_EXT(std::vector<color4>) contrast_array(const size_t size);//Return colors that are different from each other


}//namespace ork

namespace glm {

ORK_ORK_EXT(ork::o_stream&) operator<<(ork::o_stream&stream, const ork::color4&vec);

}

#endif//ORK_USE_GLM
