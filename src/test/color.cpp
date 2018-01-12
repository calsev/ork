/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/color.hpp"

#include"ork/test/catch_include.hpp"

#include"glm/vec4.hpp"

using namespace ork;


TEST_CASE("Convention for hue", "[color]") {
	const color4 d1{0.f, 0.f, 0.f, 1.f};
	const color4 r1{0.f, 0.f, 0.f, 1.f};
	REQUIRE(truncate_hue(d1, color_space::rgb) == r1);
}

TEST_CASE("RGB round trip to HEX", "[color]") {
	const color4 d1{0.1, 0.2, 0.3, 0.4};
	const string r1{ORK("1A334D66")};
	const string to{to_hex(d1, color_space::rgb)};
	REQUIRE(to == r1);
}
}
