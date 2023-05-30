#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
} // namespace Cpp98

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
} // namespace BeforeCpp17