#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>
#include <list>

template <typename Iterator>
auto advance_it(Iterator& it, size_t n)
{
    return std::input_iterator_tag{};
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);
    
        REQUIRE(*it == 4);
        //static_assert(std::is_same_v<decltype(result), std::random_access_iterator_tag>);
    }

    SECTION("input_iterator")
    {
        std::list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);

        REQUIRE(*it == 4);
        static_assert(std::is_same_v<decltype(result), std::input_iterator_tag>);
    }
}