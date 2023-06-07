#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <variant>
#include <vector>

using namespace std;

TEST_CASE("variant")
{
    std::variant<int, double, std::vector<int>> v1;

    CHECK(std::holds_alternative<int>(v1));
    CHECK(std::get<int>(v1) == 0);

    v1 = 3.14;
    v1 = std::vector{1, 2, 3};

    CHECK(std::holds_alternative<std::vector<int>>(v1));

    std::cout << std::get<std::vector<int>>(v1).size() << "\n";
    CHECK_THROWS_AS(std::get<double>(v1), std::bad_variant_access);

    auto* ptr = std::get_if<double>(&v1);
    CHECK(ptr == nullptr);

    CHECK(v1.index() == 2);
    CHECK(std::get<2>(v1) == std::vector{1, 2, 3});
}

struct Printer
{
    void operator()(int x) const  { std::cout << "int: " << x << "\n"; }
    void operator()(double x) const  { std::cout << "double: " << x << "\n"; }
    void operator()(const std::vector<int>& v) const { std::cout << "vec: " << v.size() << "\n"; }
    void operator()(std::string_view sv) const { std::cout << "string: " << sv << "\n"; }
};

// struct Lambda_56438236745
// {
//     void operator() (const std::vector<int>& v) const { std::cout << "vec: " << v.size() << "\n"; }
// };


template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

template <typename... Ts>
overload(Ts...) -> overload<Ts...>;


TEST_CASE("visiting variant")
{
    std::variant<int, double, std::vector<int>, std::string> v1;

    v1 = std::vector{1, 2, 3};
    v1 = "text"s;

    Printer printer;
    std::visit(printer, v1);

    std::variant<int, double, std::vector<int>, std::string> v2 = 3.14;
    //auto logger = [](const std::vector<int>& v) { std::cout << "vec: " << v.size() << "\n"; };

    auto logger = overload {
         [](const std::vector<int>& v) { std::cout << "vec: " << v.size() << "\n"; }, 
         [](const auto& obj) { std::cout << "Value: " << obj << "\n"; }
    };

    std::visit(logger, v2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

struct Circle
{
    int r;

    void draw() const
    {
        std::cout << "Circle(" << r << ")\n";
    }
};

struct Rectangle
{
    int w, h;

    void draw() const
    {
        std::cout << "Rect(" << w << ", " << h << ")\n";
    }
};

struct Square
{
    int size;

    void draw() const
    {
        std::cout << "Square(" << size << ")\n";
    }
};

struct Shape
{
    using TShape = std::variant<Circle, Rectangle, Square>;

    TShape shape;

    template <typename T>
    Shape(T shp) : shape(shp)
    {}

    // template <typename T>
    // Shape& operator=(const T& shp)
    // {
    //     shape = shp;
    //     return *this;
    // }

    void draw() const
    {
        std::visit([](const auto& shp) { shp.draw(); }, shape);
    }
};

TEST_CASE("VARIANT polymorphism")
{
    Shape s1 = Rectangle{100, 200};
    Shape s2 = Circle{500};

    //s2 = Rectangle{600, 700};

    std::vector<Shape> shapes;
    shapes.push_back(s1);
    shapes.push_back(s2);

    for(const auto& s : shapes)
        s.draw();
}