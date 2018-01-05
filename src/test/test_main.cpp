/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#include"ork/string_utils.hpp"

#define CATCH_CONFIG_MAIN 1
#include"ork/test/catch_include.hpp"

ORK_GLOBAL_LOG(ORK("../Logs"))

using namespace ork;

TEST_CASE("Strings are encoded", "[string]") {
	const bstring test_1{"Encode this 1!"};
	const bstring result_1{"RW5jb2RlIHRoaXMgMSE="};
	REQUIRE(encode(reinterpret_cast<const unsigned char*>(test_1.data()), test_1.size(), encoding::base_64)== result_1);
	REQUIRE(decode(result_1, encoding::base_64) == test_1);
}
