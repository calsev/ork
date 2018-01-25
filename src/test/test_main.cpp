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
	const bstring test_1{BORK("Encode this 1!")};
	const bstring result_1{BORK("RW5jb2RlIHRoaXMgMSE=")};
	REQUIRE(encode(reinterpret_cast<const unsigned char*>(test_1.data()), test_1.size(), encoding::base_64)== result_1);
	REQUIRE(decode(result_1, encoding::base_64) == test_1);
}

//We to avoid a second project to generate code
TEST_CASE("Macro Generation", "[non_test]") {
	static const size_t macro_max = 40;
	ORK_FILE_WRITE_B(ORK("generated.cpp"));
	LOOPI(macro_max) {
		fout << BORK("#define ORK_GET_ARG_") << (i / 10 > 0 ? BORK("") : BORK("0")) << i << BORK("_(");
		LOOPJ(i + 1) {
			fout << BORK('A') << j << BORK(", ");
		}
		fout << BORK("...) A") << i << BORK('\n');
	}
}
