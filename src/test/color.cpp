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
bool within_hex(const color4& v1, const color4& v2) {
	LOOPVIG(v1) {
		if(!within_hex(v1[i], v2[i])) {
			return false;
		}
	}
	return true;
}
bool within_3(const float v1, const float v2) {
	return std::abs(v1 - v2) < 0.001;
}
bool within_3(const color4& v1, const color4& v2) {
	LOOPVIG(v1) {
		if(!within_3(v1[i], v2[i])) {
			return false;
		}
	}
	return true;
}
bool within_5(const float v1, const float v2) {
	return std::abs(v1 - v2) < 0.00001;
}
bool within_5(const color4& v1, const color4& v2) {
	LOOPVIG(v1) {
		if(!within_3(v1[i], v2[i])) {
			return false;
		}
	}
	return true;
}


TEST_CASE("Convention for hue", "[color]") {
	const color4 d1{0.0, 0.0, 0.0, 1.0};
	const color4 r1{0.0, 0.0, 0.0, 1.0};
	REQUIRE(truncate_hue(d1, color_space::rgb) == r1);
}


TEST_CASE("RGB To HSL", "[color]") {
	const color4 red_e{0.0, 1.0, 0.5, 1.0};
	const color4 green_e{1.0 / 3.0, 1.0, 0.5, 1.0};
	const color4 blue_e{2.0 / 3.0, 1.0, 0.5, 1.0};
	const color4 white_e{0.0, 0.0, 1.0, 1.0};
	const color4 black_e{0.0, 0.0, 0.0, 1.0};

	const color4 red_r{convert(red, color_space::rgb, color_space::hsl)};
	const color4 green_r{convert(green, color_space::rgb, color_space::hsl)};
	const color4 blue_r{convert(blue, color_space::rgb, color_space::hsl)};
	const color4 white_r{convert(white, color_space::rgb, color_space::hsl)};
	const color4 black_r{convert(black, color_space::rgb, color_space::hsl)};

	REQUIRE(within_5(red_e, red_r));
	REQUIRE(within_5(green_e, green_r));
	REQUIRE(within_5(blue_e, blue_r));
	REQUIRE(within_5(white_e, white_r));
	REQUIRE(within_5(black_e, black_r));
}

TEST_CASE("RGB To HSV", "[color]") {
	const color4 red_e{0.0, 1.0, 1.0, 1.0};
	const color4 green_e{1.0 / 3.0, 1.0, 1.0, 1.0};
	const color4 blue_e{2.0 / 3.0, 1.0, 1.0, 1.0};
	const color4 white_e{0.0, 0.0, 1.0, 1.0};
	const color4 black_e{0.0, 0.0, 0.0, 1.0};

	const color4 red_r{convert(red, color_space::rgb, color_space::hsv)};
	const color4 green_r{convert(green, color_space::rgb, color_space::hsv)};
	const color4 blue_r{convert(blue, color_space::rgb, color_space::hsv)};
	const color4 white_r{convert(white, color_space::rgb, color_space::hsv)};
	const color4 black_r{convert(black, color_space::rgb, color_space::hsv)};

	REQUIRE(within_5(red_e, red_r));
	REQUIRE(within_5(green_e, green_r));
	REQUIRE(within_5(blue_e, blue_r));
	REQUIRE(within_5(white_e, white_r));
	REQUIRE(within_5(black_e, black_r));
}

TEST_CASE("RGB round trip to HEX", "[color]") {
	const color4 d1{0.1, 0.2, 0.3, 0.4};
	const string r1{ORK("1A334D66")};
	const string to{to_hex(d1, color_space::rgb)};
	const color4 from{from_hex(to, color_space::rgb)};
	REQUIRE(to == r1);
	REQUIRE(within_hex(from, d1));
}
