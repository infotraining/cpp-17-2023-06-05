#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

namespace BeforeCpp17
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

tuple<int, int, double> calc_stats(const vector<int>& data)
{
    auto [min_pos, max_pos] = minmax_element(data.begin(), data.end()); // sb

    double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

    return make_tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    int min, max;
    // double avg;

    tie(min, max, std::ignore) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == Catch::Approx(665));
    // REQUIRE(avg == Catch::Approx(141.333));
}

TEST_CASE("Since C++17")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    auto [min, max, avg] = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == Catch::Approx(665));
    REQUIRE(avg == Catch::Approx(141.333));
}

std::array<int, 3> get_coord()
{
    return {10, 20, 30};
}

struct ErrorCode
{
    int ec;
    std::string m;
};

ErrorCode open_file(const char* filename)
{
    if (filename == ""s)
        return ErrorCode{13, "Filename cannot be empty"};

    return ErrorCode{};
}

TEST_CASE("structured bindings")
{
    SECTION("native arrays")
    {
        int tab[] = {10, 20};

        auto [x, y] = tab; // copy items from tab to x, y

        CHECK(x == 10);
        CHECK(y == 20);

        x += 5;

        CHECK(x == 15);
        CHECK(tab[0] == 10);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();
    }

    SECTION("struct")
    {
        const auto [error_code, error_message] = open_file("");

        CHECK(error_code == 13);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    SECTION("sb")
    {
        auto&& [hours, minutes, seconds] = Timestamp{10, 20, 30};
    }

    SECTION("is interpreted as")
    {
        auto&& entity = Timestamp{};
        auto& hours = entity.h;
        auto& minutes = entity.m;
        auto& seconds = entity.s;
    }
}

TEST_CASE("sb effeciency")
{
    SECTION("rvalue")
    {
        auto [hours, minutes, seconds] = Timestamp{10, 20, 30}; // rvalue -> no copy

        // auto entity = Timestamp{}; // RVO
    }

    SECTION("lvalue")
    {

        Timestamp timepoint{10, 20, 44};

        auto& [hours, minutes, seconds] = timepoint; // lvalue -> copy
    }
}

TEST_CASE("sb - move semantics")
{
    auto [error_code, error_message] = open_file(""); // RVO

    SECTION("move only when auto without &&")
    {
        ErrorCode ec = open_file("");

        auto [error_code, error_message] = std::move(ec); 

        CHECK(ec.m == ""s);
        CHECK(error_message != ""s);
    }

    SECTION("potential issue")
    {
        ErrorCode ec = open_file("");

        auto& [error_code, error_message] = ec; 

        std::string target = std::move(error_message);
    }
}

TEST_CASE("use case")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}, {3, "three"}};

    auto [pos, was_inserted] = dict.insert(std::pair{1, "jeden"});    
    CHECK(was_inserted == false);

    for(const auto& [key, value] : dict)
    {
        std::cout << key << " - " << value << "\n";
    }

    std::vector vec = {1543, 4, 543};

    for(auto [it, index] = std::pair{vec.begin(), 0}; it != vec.end(); ++it, ++index)
    {
        std::cout << index << " - " << *it << "\n";
    }
}

//////////////////////////////////////////////////////////////////////////
// tuple protocol

class Person
{
    std::string first_name_;
    std::string last_name_;
    int age_;
    
public:
    Person(std::string fn, std::string ln, int age) : first_name_{std::move(fn)}, last_name_{std::move(ln)}, age_{age}
    {}

    const std::string& first_name() const
    {
        return first_name_;
    }

    const std::string& last_name() const
    {
        return last_name_;
    }
};

/////////////////////////////////////////////////////
// tuple-like protocol for Person

// step 1
template <>
struct std::tuple_size<Person>
{
    static constexpr size_t value = 2;
};

// step 2
template <size_t Index>
struct std::tuple_element<Index, Person>
{
    using type = std::string;
};

// template <>
// struct std::tuple_element<1, Person>
// {
//     using type = std::string;
// };

// step 3
template <size_t Index>
decltype(auto) get(const Person&);

template <>
decltype(auto) get<0>(const Person& p)
{
    return p.first_name();
}

template <>
decltype(auto) get<1>(const Person& p)
{
    return p.last_name();
}

TEST_CASE("sb for custom types")
{
    const Person p{"Jan", "Kowalski", 33};

    const auto& [first_name, last_name] = p; // now it works

    CHECK(first_name == "Jan");
    CHECK(last_name == "Kowalski");
}

struct Base
{
    int base1;
    int base2;
};

struct Derived  : Base
{
    void foo() {}
};

TEST_CASE("sb & inheritance")
{
    auto [a, b] = Derived{10, 30};
}