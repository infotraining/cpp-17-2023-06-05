#include "gadget.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("static inline")
{
    Gadget g1;
    REQUIRE(g1.id() == 1);

    Gadget g2;
    REQUIRE(g2.id() == 2);
}