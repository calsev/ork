/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/color.hpp"
#include"ork/glm.hpp"

#include"ork/test/catch_include.hpp"

#include"glm/vec4.hpp"

using namespace ork;


bool within_hex(const float v1, const float v2) {
	return std::abs(v1 - v2) < 0.5 / 255.0;
}


TEST_CASE("Convention for hue", "[color]") {
	const color4 d1{0.f, 0.f, 0.f, 1.f};
	const color4 r1{0.f, 0.f, 0.f, 1.f};
	REQUIRE(truncate_hue(d1, color_space::rgb) == r1);
}

TEST_CASE("RGB Conversion", "[color]") {
	const color4 rgb{0.2f, 0.3f, 0.4f, 0.5f};
	const color4 r1{0.5850f, 0.3333f, 0.3000f, 0.5f};//HSL
	const color4 r2{0.5850f, 0.5000f, 0.4000f, 0.5f};//HSV
	const color4 c1{convert(rgb, color_space::rgb, color_space::hsl)};
	const color4 c2{convert(rgb, color_space::rgb, color_space::hsv)};
}

TEST_CASE("RGB round trip to HEX", "[color]") {
	const color4 d1{0.1, 0.2, 0.3, 0.4};
	const string r1{ORK("1A334D66")};
	const string to{to_hex(d1, color_space::rgb)};
	const color4 from{from_hex(to, color_space::rgb)};
	REQUIRE(to == r1);
	LOOPVIG(d1) {
		REQUIRE(within_hex(from[i], d1[i]));
	}
}
}
