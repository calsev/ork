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
    static const size_t macro_max = 64;
    ORK_FILE_WRITE_B(ORK("generated.cpp"));

    LOOPI(macro_max)
    {
        fout << BORK("#define ORK_ARG_") << i << BORK("(");
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

    fout << BORK("\n\n");
    LOOPI(macro_max)
    {
        fout << BORK("#define ORK_INC_") << i << BORK(" ") << i + 1 << BORK('\n');
    }

    fout << BORK("\n");
    LOOPI(macro_max)
    {
        fout << BORK("#define ORK_DEC_") << i + 1 << BORK(" ") << i << BORK('\n');
    }
}


TEST_CASE("MAP Macro Test", "[pp_meta]")
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

    REQUIRE(ORK_BOOL(0) == 0);
    REQUIRE(ORK_BOOL(1) == 1);
    REQUIRE(ORK_BOOL(a) == 1);
    static const char a = 'a';
    static const char b = 'b';
#define A() a
#define B() b
    REQUIRE(ORK_BOOL(F()) == 0);
    REQUIRE(ORK_BOOL(T()) == 1);
    REQUIRE(ORK_BOOL(A()) == 1);

    REQUIRE(ORK_IF(0)(a, b) == b);
    REQUIRE(ORK_IF(1)(a, b) == a);
    REQUIRE(ORK_IF(a)(a, b) == a);
    REQUIRE(ORK_IF(B())(a, b) == a);

    REQUIRE(1 ORK_WHEN(0)(+1) == 1);
    REQUIRE(1 ORK_WHEN(1)(+1) == 2);

	REQUIRE(ORK_UNPAREN(a) == a);
	REQUIRE(ORK_UNPAREN(A()) == a);
	REQUIRE(ORK_UNPAREN((a)) == a);
	REQUIRE(ORK_STR(ORK_UNPAREN((a))) == bstring("a"));

    // clang-format off
#define ORK_SQUARE(DATA, I, A) A*A
#define ORK_ADD(DATA, I, A, B) A + B
    // clang-format on

    const ork::bstring square_list{ORK_STR(ORK_MAP(ORK_SQUARE, ORK_ADD, data, 1, 2))};
    REQUIRE(square_list == BORK("1*1 + 2*2"));
    int val = ORK_MAP(ORK_SQUARE, ORK_ADD, data, 1, 2);
    REQUIRE(val == 5);

    // clang-format off
#define CALL(DATA, I, ARG) DATA--ARG--I
#define STITCH(DATA, I, X, Y) (X *DATA-I* Y)
    // clang-format on
    const ork::bstring assoc_list{ORK_STR(ORK_MAP(CALL, STITCH, my, a, b, c))};
    REQUIRE(assoc_list == BORK("(my--a--0 *my-0* (my--b--1 *my-1* my--c--2))"));

    // clang-format off
#define DATA (One, Two)
#undef CALL
#define CALL(D, I, ARG) I--ARG--I
#undef STITCH
#define STITCH(D, I, X, Y) (X *D, I* Y)
    // clang-format on
    const ork::bstring expand_list{ORK_STR(ORK_MAP(CALL, STITCH, DATA, a, b, c))};
    REQUIRE(expand_list == BORK("(0--a--0 *(One, Two), 0* (1--b--1 *(One, Two), 1* 2--c--2))"));

	// clang-format off
#undef CALL
#define CALL(D, I, ARG) ORK_ARG_0 D--ARG--ORK_ARG_1 D
#undef STITCH
#define STITCH(D, I, X, Y) (X *ORK_EVAL D, I* Y)
	// clang-format on
	const ork::bstring expand_call_list{ORK_STR(ORK_MAP(CALL, STITCH, DATA, a, b, c))};
	REQUIRE(expand_call_list == BORK("(One--a--Two *One, Two, 0* (One--b--Two *One, Two, 1* One--c--Two))"));
}


TEST_CASE("Macro Test", "[pp_meta]")
{
    const int arg_1 = ORK_NUM_ARG(a);
    REQUIRE(arg_1 == 1);

    const int arg_3 = ORK_NUM_ARG(a, b, c);
    REQUIRE(arg_3 == 3);
}
