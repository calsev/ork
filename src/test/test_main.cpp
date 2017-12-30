/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#include"ork/string_utils.hpp"

#define CATCH_CONFIG_MAIN 1
#include"catch.hpp"

ORK_GLOBAL_LOG(ORK("../Logs"))

using namespace ork;

TEST_CASE("Strings are encoded", "[string]") {
	const bstring test1{"Encode this 1!"};
	REQUIRE(encode(reinterpret_cast<const unsigned char*>(test1.data()), test1.size(), encoding::base_64)=="RW5jb2RlIHRoaXMgMSE=");
}
