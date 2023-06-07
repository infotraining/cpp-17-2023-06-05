#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

using namespace std::literals;

// variable template
template <typename T>
static constexpr T pi = 3.141592653589793238;

struct Circle
{
    int radius;

    void draw() const
    {
        std::cout << "Drawing Circle with r: " << radius << "\n";
    }
};

struct Rectangle
{
    int width, height;

    void draw() const
    {
        std::cout << "Drawing Rectangle with w: " << width << " & h: " << height << "\n";
    }
};

struct Square
{
    int size;

    void draw() const
    {
        std::cout << "Drawing Square with size: " << size << "\n";
    }
};


///////////////////////////////////////////
// overload

template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

// deduction guide
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = std::variant<Circle, Rectangle, Square>;

    std::vector<Shape> shapes = {Circle{1}, Square{10}, Rectangle{10, 1}};

    double total_area{};

    // TODO
    SECTION("return value")
    {
        auto area_calculator = overload{
            [](const Circle& c) -> double
            { return c.radius * c.radius * pi<decltype(total_area)>; },
            [](const Rectangle& r) -> double
            { return r.width * r.height; },
            [](const Square& s) -> double
            { return s.size * s.size; }};

        for (const auto& shape : shapes)
        {
            total_area += std::visit(area_calculator, shape);
        }

        REQUIRE_THAT(total_area, Catch::Matchers::WithinRel(113.14, 0.01));
    }

    SECTION("captured arg")
    {
        auto area_calculator = overload{
            [&total_area](const Circle& c)
            { total_area += c.radius * c.radius * pi<decltype(total_area)>; },
            [&total_area](const Rectangle& r)
            { total_area += r.width * r.height; },
            [&total_area](const Square& s)
            { total_area += s.size * s.size; }};
        
        for (const auto& shape : shapes)
        {
            std::visit(area_calculator, shape);
        }

        REQUIRE_THAT(total_area, Catch::Matchers::WithinRel(113.14, 0.01));
    }
}