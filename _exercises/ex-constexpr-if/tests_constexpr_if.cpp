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
    if constexpr(std::is_base_of_v< 
            std::random_access_iterator_tag, 
            typename std::iterator_traits<Iterator>::iterator_category           
        >)
    {
        it += n;
        return std::random_access_iterator_tag {};
    }
    else
    {
        for (auto i = 0; i < n; i++) it++;
        return std::input_iterator_tag{};
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator - it += n")
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);

        REQUIRE(*it == 4);
        //static_assert(std::is_same_v<decltype(result), std::random_access_iterator_tag>);
    }

    SECTION("input_iterator - n times ++it")
    {
        std::list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);

        REQUIRE(*it == 4);
        static_assert(std::is_same_v<decltype(result), std::input_iterator_tag>);
    }
}