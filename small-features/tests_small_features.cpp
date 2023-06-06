#include "gadget.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <functional>

using namespace std;

TEST_CASE("static inline")
{
    Gadget g1;
    REQUIRE(g1.id() == 1);

    Gadget g2;
    REQUIRE(g2.id() == 2);
}

struct Data
{
    int a;
    double b;
    std::string name;

    void print() const
    {
        std::cout << "Data(a=" << a << ", b=" << b << ",name=" << name << ")\n";
    }
};

struct BigData : Data
{
    std::vector<int> vec;
};

template <size_t N>
struct String : std::string
{};

struct ManyStrings : String<1>, String<2>
{
};

TEST_CASE("aggregates")
{
    Data d1{1, 3.14, "data"};
    Data d2{1};

    d1.print();
    d2.print();

    BigData dataset1{{1, 3.14, "base-data"}, {1, 2, 3, 4}};

    dataset1.print();

    static_assert(std::is_aggregate_v<BigData>);

    ManyStrings ms{{"abc"}, {"defgh"}};

    std::cout << ms.String<1>::size() << " - " << ms.String<2>::size() << "\n";
}

[[nodiscard]] std::errc open_file(const std::string& name)
{
    return std::errc::bad_file_descriptor;
}

[[deprecated]] int old_foo() // since C++14
{
    return 665;
}

namespace [[deprecated]] Legacy
{
    int bar()
    {
        return 665;
    }
}

TEST_CASE("attributes")
{
    [[maybe_unused]] auto result = open_file("evil");

    if (std::errc error_code = open_file("evil"); error_code == std::errc::bad_file_descriptor)
    {
        std::cout << "std::errc::bad_file_descriptor ERROR!!!\n";
    }

    old_foo();
    Legacy::bar();
}

///////////////////////////////////////////////////////////////////
// lambdas in C++17

struct Object
{
    std::string name;

    auto get_callback()
    {
        return [*this] { std::cout << "Gadget: " << name << "\n"; }; // capturing copy of object
    }

    ~Object()
    {
        std::cout << "~Object(" << name << ")\n";
    }
};

TEST_CASE("capturing *this")
{
    std::function<void()> callback;

    {
        Object object{"object"};
        std::cout << "------------------\n";
        callback = object.get_callback();
        std::cout << "------------------\n";
    }

    callback();
}

TEST_CASE("constexpr lambda")
{
    auto square = [](int x) { return x * x; }; // lambda is implicitly declared as constexpr

    static_assert(square(8) == 64);

    int native_array[square(10)];
    static_assert(std::size(native_array) == 100);

    std::array<int, square(8)> numbers = {};
    static_assert(numbers.size() == 64);
}
template <size_t N>
constexpr auto create_array()
{
    auto square = [](int x) { return x * x; };

    std::array<int, square(N)> data = {};

    for(size_t i = 0; i < square(N); ++i)
        data[i] = square(i);
    
    return data;
}


namespace Cpp20
{
    template <typename Iterator, typename Predicate>
    constexpr Iterator find_if(Iterator first, Iterator last, Predicate pred)
    {
        while(first != last)
        {
            if (pred(*first))
            {
                return first;
            }
            ++first;
        }

        return last;
    }
}

constexpr int anytime_foo(int x)
{
    return 42 * x;
}

TEST_CASE("constexpr world")
{
    constexpr auto lookup_table_of_squares = create_array<20>();

    static_assert(lookup_table_of_squares[9] == 81);

    constexpr int size_of_buffer = *Cpp20::find_if(lookup_table_of_squares.begin(), lookup_table_of_squares.end(), [](int x) { return x > 100; });

    std::cout << "size of buffer: " << size_of_buffer << "\n";

    int native_array[size_of_buffer];

    constexpr int x = 2;
    constexpr int my_value = anytime_foo(x);
}