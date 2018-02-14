/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include "ork/geometry.hpp"
#include "ork/glm.hpp"

#include "ork/test/catch_include.hpp"

#include "glm/vec3.hpp"

using namespace ork;


TEST_CASE("Length conversion", "[geometry]")
{
    const double dmm = 50.8;
    const double din = 3.0;

    const double rin = 2.0;
    const double rmm = 76.2;

    REQUIRE(ork::GLM::equal(mm2inch(dmm), rin));
    REQUIRE(ork::GLM::equal(inch2mm(din), rmm));
}

TEST_CASE("Simple angles", "[geometry]")
{
    const double angle1 = 136.0;
    const double angle2 = 395.0;
    const double angle3 = -172.0;
    const double angle4 = 2.6 * M_PI;

    const double sangle1 = 136.0;
    const double sangle2 = 35.0;
    const double sangle3 = 188.0;
    const double sangle4 = 0.6 * M_PI;

    REQUIRE(ork::GLM::equal(simple_angle<angle::degree>(angle1), sangle1));
    REQUIRE(ork::GLM::equal(simple_angle<angle::degree>(angle2), sangle2));
    REQUIRE(ork::GLM::equal(simple_angle(angle3, angle::degree), sangle3));
    REQUIRE(ork::GLM::equal(simple_angle(angle4, angle::radian), sangle4));
}
}
