/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/ork.hpp"
#include "ork/string_utils.hpp"

#define CATCH_CONFIG_MAIN 1
#include "ork/test/catch_include.hpp"

ORK_GLOBAL_LOG(ORK("../Logs"))

using namespace ork;

TEST_CASE("Strings are encoded", "[string]")
{
    const bstring test_1{BORK("Encode this 1!")};
    const bstring result_1{BORK("RW5jb2RlIHRoaXMgMSE=")};
    REQUIRE(
        encode(
            reinterpret_cast<const unsigned char*>(test_1.data()),
            test_1.size(),
            encoding::base_64) == result_1);
    REQUIRE(decode(result_1, encoding::base_64) == test_1);
}


// We to avoid a second project to generate code
TEST_CASE("Macro Generation", "[pp_meta]")
{
    static const size_t macro_max = 40;
    ORK_FILE_WRITE_B(ORK("generated.cpp"));

    LOOPI(macro_max)
    {
        fout << BORK("#define ORK_GET_ARG_")
             << (i / 10 > 0 ? BORK("") : BORK("0")) << i << BORK("_(");
        LOOPJ(i + 1)
        {
            fout << BORK('A') << j << BORK(", ");
        }
        fout << BORK("...) A") << i << BORK('\n');
    }

    fout << BORK("\n");
    fout << BORK("#define ORK_ARG_N_(");
    LOOPI(macro_max + 1)
    {
        fout << BORK('A') << i + 1 << BORK(", ");
    }
    fout << BORK("...) A") << macro_max + 1 << BORK('\n');

    fout << BORK("\n");
    fout << BORK("#define ORK_DESCENDING_N_ ");
    LOOPRI(macro_max + 1)
    {
        fout << i << (i > 0 ? BORK(", ") : BORK(""));
    }

    const int arg_1 = ORK_NUM_ARG(a);
    REQUIRE(arg_1 == 1);

    const int arg_3 = ORK_NUM_ARG(a, b, c);
    REQUIRE(arg_3 == 3);
}
TEST_CASE("Macro Map", "[pp_meta]")
{
    REQUIRE(ORK_IF_(1)(4, 5) == 4);
    REQUIRE(ORK_IF_(0)(4, 5) == 5);
#define T() 1
#define F() 0
    REQUIRE(ORK_IF_(T())(4, 5) == 4);
    REQUIRE(ORK_IF_(F())(4, 5) == 5);

    REQUIRE(ORK_CHECK_1_() == 0);
    REQUIRE(ORK_CHECK_1_(1) == 0);
    REQUIRE(ORK_CHECK_1_(1, 2) == 2);
    REQUIRE(ORK_CHECK_1_(1, 2, 3) == 2);
#define A1() 1
#define A2() 1, 2
#define A3() 1, 2, 3
    REQUIRE(ORK_CHECK_1_(A1()) == 0);
    REQUIRE(ORK_CHECK_1_(A2()) == 2);
    REQUIRE(ORK_CHECK_1_(A3()) == 2);

    REQUIRE(ORK_CHECK_1_(ORK_APPEND_1_(1)) == 1);
    REQUIRE(ORK_CHECK_1_(ORK_APPEND_1_(1, 2)) == 2);
    REQUIRE(ORK_CHECK_1_(ORK_APPEND_1_(A1())) == 1);
    REQUIRE(ORK_CHECK_1_(ORK_APPEND_1_(A2())) == 2);

    REQUIRE(ORK_IS_PAREN(()) == 1);
    REQUIRE(ORK_IS_PAREN(~) == 0);
#define P() ()
#define NP() ~
    REQUIRE(ORK_IS_PAREN(P()) == 1);
    REQUIRE(ORK_IS_PAREN(NP()) == 0);

	REQUIRE(ORK_NOT(0) == 1);
	REQUIRE(ORK_NOT(1) == 0);
	REQUIRE(ORK_NOT(F()) == 1);
	REQUIRE(ORK_NOT(T()) == 0);
}
