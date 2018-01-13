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


#define CONVERSIONS(CS1, CS2)\
const color4 red_r{convert(red(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 yellow_r{convert(yellow(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 green_r{convert(green(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 cyan_r{convert(cyan(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 blue_r{convert(blue(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 magenta_r{convert(magenta(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 white_r{convert(white(color_space::CS1), color_space::CS1, color_space::CS2)};\
const color4 black_r{convert(black(color_space::CS1), color_space::CS1, color_space::CS2)};


TEST_CASE("RGB To HSL", "[color]") {
	CONVERSIONS(rgb, hsl)

	REQUIRE(within_5(red_r, red(color_space::hsl)));
	REQUIRE(within_5(yellow_r, yellow(color_space::hsl)));
	REQUIRE(within_5(green_r, green(color_space::hsl)));
	REQUIRE(within_5(cyan_r, cyan(color_space::hsl)));
	REQUIRE(within_5(blue_r, blue(color_space::hsl)));
	REQUIRE(within_5(magenta_r, magenta(color_space::hsl)));
	REQUIRE(within_5(white_r, white(color_space::hsl)));
	REQUIRE(within_5(black_r, black(color_space::hsl)));

}

TEST_CASE("RGB To HSV", "[color]") {
	CONVERSIONS(rgb, hsv)

	REQUIRE(within_5(red_r, red(color_space::hsv)));
	REQUIRE(within_5(yellow_r, yellow(color_space::hsv)));
	REQUIRE(within_5(green_r, green(color_space::hsv)));
	REQUIRE(within_5(cyan_r, cyan(color_space::hsv)));
	REQUIRE(within_5(blue_r, blue(color_space::hsv)));
	REQUIRE(within_5(magenta_r, magenta(color_space::hsv)));
	REQUIRE(within_5(white_r, white(color_space::hsv)));
	REQUIRE(within_5(black_r, black(color_space::hsv)));
}

TEST_CASE("HSL To RGB", "[color]") {
	CONVERSIONS(hsl, rgb)

	REQUIRE(within_5(red_r, red(color_space::rgb)));
	REQUIRE(within_5(yellow_r, yellow(color_space::rgb)));
	REQUIRE(within_5(green_r, green(color_space::rgb)));
	REQUIRE(within_5(cyan_r, cyan(color_space::rgb)));
	REQUIRE(within_5(blue_r, blue(color_space::rgb)));
	REQUIRE(within_5(magenta_r, magenta(color_space::rgb)));
	REQUIRE(within_5(white_r, white(color_space::rgb)));
	REQUIRE(within_5(black_r, black(color_space::rgb)));
}

TEST_CASE("HSL To HSV", "[color]") {
	CONVERSIONS(hsl, hsv)

	REQUIRE(within_5(red_r, red(color_space::hsv)));
	REQUIRE(within_5(yellow_r, yellow(color_space::hsv)));
	REQUIRE(within_5(green_r, green(color_space::hsv)));
	REQUIRE(within_5(cyan_r, cyan(color_space::hsv)));
	REQUIRE(within_5(blue_r, blue(color_space::hsv)));
	REQUIRE(within_5(magenta_r, magenta(color_space::hsv)));
	REQUIRE(within_5(white_r, white(color_space::hsv)));
	REQUIRE(within_5(black_r, black(color_space::hsv)));
}

TEST_CASE("HSV To RGB", "[color]") {
	CONVERSIONS(hsv, rgb)

	REQUIRE(within_5(red_r, red(color_space::rgb)));
	REQUIRE(within_5(yellow_r, yellow(color_space::rgb)));
	REQUIRE(within_5(green_r, green(color_space::rgb)));
	REQUIRE(within_5(cyan_r, cyan(color_space::rgb)));
	REQUIRE(within_5(blue_r, blue(color_space::rgb)));
	REQUIRE(within_5(magenta_r, magenta(color_space::rgb)));
	REQUIRE(within_5(white_r, white(color_space::rgb)));
	REQUIRE(within_5(black_r, black(color_space::rgb)));
}

TEST_CASE("HSV To HSL", "[color]") {
	CONVERSIONS(hsv, hsl)

	REQUIRE(within_5(red_r, red(color_space::hsl)));
	REQUIRE(within_5(yellow_r, yellow(color_space::hsl)));
	REQUIRE(within_5(green_r, green(color_space::hsl)));
	REQUIRE(within_5(cyan_r, cyan(color_space::hsl)));
	REQUIRE(within_5(blue_r, blue(color_space::hsl)));
	REQUIRE(within_5(magenta_r, magenta(color_space::hsl)));
	REQUIRE(within_5(white_r, white(color_space::hsl)));
	REQUIRE(within_5(black_r, black(color_space::hsl)));
}

TEST_CASE("RGB round trip to HEX", "[color]") {
	const color4 d1{0.1, 0.2, 0.3, 0.4};
	const string r1{ORK("1A334D66")};
	const string to{to_hex(d1, color_space::rgb)};
	const color4 from{from_hex(to, color_space::rgb)};
	REQUIRE(to == r1);
	REQUIRE(within_hex(from, d1));
}

#define TO_HEX(COLOR, SPACE) const ork::string ORK_CAT(COLOR,_r){to_hex(COLOR(color_space::SPACE), color_space::SPACE)}

TEST_CASE("HSL to HEX", "[color]") {
	TO_HEX(red, hsl);
	TO_HEX(yellow, hsl);
	TO_HEX(green, hsl);
	TO_HEX(cyan, hsl);
	TO_HEX(blue, hsl);
	TO_HEX(magenta, hsl);
	TO_HEX(white, hsl);
	TO_HEX(black, hsl);

	REQUIRE(red_r == ORK("FF0000FF"));
	REQUIRE(yellow_r == ORK("FFFF00FF"));
	REQUIRE(green_r == ORK("00FF00FF"));
	REQUIRE(cyan_r == ORK("00FFFFFF"));
	REQUIRE(blue_r == ORK("0000FFFF"));
	REQUIRE(magenta_r == ORK("FF00FFFF"));
	REQUIRE(white_r == ORK("FFFFFFFF"));
	REQUIRE(black_r == ORK("000000FF"));
}

#define FROM_HEX(COLOR, STR, SPACE) const color4 ORK_CAT(COLOR,_r){from_hex(ORK(STR), color_space::SPACE)}

TEST_CASE("HEX to HSL", "[color]") {
	FROM_HEX(red, "FF0000FF", hsl);
	FROM_HEX(yellow, "FFFF00FF", hsl);
	FROM_HEX(green, "00FF00FF", hsl);
	FROM_HEX(cyan, "00FFFFFF", hsl);
	FROM_HEX(blue, "0000FFFF", hsl);
	FROM_HEX(magenta, "FF00FFFF", hsl);
	FROM_HEX(white, "FFFFFFFF", hsl);
	FROM_HEX(black, "000000FF", hsl);

	REQUIRE(red_r == red(color_space::hsl));
	REQUIRE(yellow_r == yellow(color_space::hsl));
	REQUIRE(green_r == green(color_space::hsl));
	REQUIRE(cyan_r == cyan(color_space::hsl));
	REQUIRE(blue_r == blue(color_space::hsl));
	REQUIRE(magenta_r == magenta(color_space::hsl));
	REQUIRE(white_r == white(color_space::hsl));
	REQUIRE(black_r == black(color_space::hsl));
}
