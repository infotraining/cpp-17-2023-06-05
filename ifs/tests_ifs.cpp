#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 543, 235, 534, 665, 654, 3456};

    auto pos = std::find(begin(vec), end(vec), 665);

    if (pos != end(vec))
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else
    {
        std::cout << "Item not found!\n";
        assert(pos == end(vec));
    }
}