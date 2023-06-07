#include <algorithm>
#include <array>
#include <atomic>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using namespace std::literals;

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

TEST_CASE("optional")
{
    std::optional<int> opt_int;
    CHECK(opt_int.has_value() == false);

    opt_int = 42;
    CHECK(opt_int.has_value() == true);

    if (opt_int > 2)
    {
    }

    if (opt_int)
    {
        std::cout << *opt_int << "\n";
    }

    // opt_int.reset();
    opt_int = std::nullopt;

    CHECK_THROWS_AS(opt_int.value(), std::bad_optional_access);

    std::optional<std::vector<int>> opt_vec(std::in_place, 10, 1);
    print_all(*opt_vec, "in opt_vec");

    auto vec_target = std::move(*opt_vec);
    opt_vec = std::nullopt; // for safety reasons

    if (opt_vec)
    {
        opt_vec->size();
    }

    std::optional<bool> opt_flag{false};

    if (opt_flag) // query if optional has state - operator bool
    {
        std::cout << "opt_flag has state"
                  << "\n";
    }

    if (opt_flag == false) // if optional has state compare with bool{false} - operator==
    {
        std::cout << "flag is false"
                  << "\n";
    }
}
