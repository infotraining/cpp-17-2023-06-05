#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <tuple>
#include <list>
#include <array>

using namespace std;

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

// namespace Cpp20
// {
//     void deduce1(auto arg)
//     {}
// }

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)  // T&& - universal reference
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    deduce1(x);        // T = int
    auto ax1 = 10;     // int

    const int cx = 10;
    deduce1(cx);       // T = int
    auto ax2 = cx;     // int

    int& ref_x = x;
    deduce1(ref_x);    // T = int
    auto ax3 = ref_x;  // int

    const int& cref_x = cx;
    deduce1(cref_x);   // T = int
    auto ax4 = cref_x; // int

    int tab[10];
    deduce1(tab);      // T = int*
    auto ax5 = tab;    // int*

    deduce1(foo);      // T = void(*)(int)
    auto ax6 = foo;    // void(*ax6)(int)
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    deduce2(x);         // T = int; int& arg
    auto& ax1 = x;      // int&
    
    const int cx = 10;
    deduce2(cx);        // T = const int; const int& arg
    auto& ax2 = cx;     // const int&

    int& ref_x = x;
    deduce2(ref_x);     // T = int; int& arg
    auto& ax3 = ref_x;  // int&

    const int& cref_x = cx;
    deduce2(cref_x);    // T = const int; const int& arg
    auto& ax4 = cref_x; // const int&

    int tab[10];
    deduce2(tab);       // T = int[10]; T(&arg)[10]
    auto& ax5 = tab;    // int(&)[10]

    deduce2(foo);       // T = void(int); void(&arg)(int)
    auto& ax6 = foo;    // void(&ax6)(int)
}

TEST_CASE("Template Argument Deduction - case 3")
{
    deduce3(42);     // T = int; int&& arg;
    auto&& ax1 = 42; // int&&

    int x = 42;
    deduce3(x);      // T = int&; int& && -> int&
    auto&& ax2 = x;  // int&
}

TEST_CASE("literals")
{
    auto txt = "text";  // const char*
    auto str = "text"s; // std::string("text")
    auto timespan = 1s; // std::chrono::seconds(1)
}

//////////////////////////////////////////////////
// CTAD


template <typename T1 = int, typename T2 = int>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair(const T1& f, const T2& s) : fst(f), snd(s)
    {}
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*) -> ValuePair<std::string, std::string>;

// helper function - required in C++98-14
template <typename T1, typename T2>
ValuePair<T1, T2> make_value_pair(T1 f, T2 s)
{
    return ValuePair<T1, T2>(f, s);
}

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1(10, 3.14);

    auto vp2 = make_value_pair(10, 3.14);

    ValuePair<> vp3(0, 1);

    // since C++17 - CTAD
    ValuePair vp4(10, "text");    // ValuePair<int, const char*>

    ValuePair vp5{3.14, "text"s}; // ValuePair<double, std::string>

    int tab[10] = {1, 2, 3};
    std::vector vec{1, 2, 3};
    ValuePair vp6{tab, vec};  // ValuePair<int*, std::vector<int>

    ValuePair vp7{"text", "abc"}; //  ValuePair<string, string> - because of deduction guide: ValuePair(const char*, const char*) -> ValuePair<std::string, std::string>;

    std::tuple t1{1, 3.4, "text"}; // std::tuple<int, double, const char*>
    std::tuple t2{t1}; // cc -> std::tuple<int, double, const char*>
}

TEST_CASE("CTAD - special copy case")
{
    std::vector vec = {1, 2, 3}; // vector<int>

    REQUIRE(vec == std::vector{1, 2, 3});

    std::vector data1{1, 4, 5, 6, 8}; // vector<int>
    std::vector data2{data1}; // special case - CTAD with copy -> vector<int>
    std::vector data3{data1, data1}; // std::vector<std::vector<int>>
}

///////////////////////////////////////////////////////////////////

template <typename T>
struct Data
{
    T value;
};

template <typename T>
Data(T) -> Data<T>;

TEST_CASE("aggregates & CTAD")
{
    Data d1{42}; // Data<int>

    Data d2{std::vector{1, 2, 3}}; // Data<std::vector<int>>
}

int foobar(int x)
{
    return 42 * x;
}

//////////////////////////////////////////////////////
TEST_CASE("std library & CTAD")
{
    SECTION("pair")
    {
        std::pair p1{10, "text"}; // std::pair<int, const char*>
    }

    SECTION("tuple")
    {
        std::tuple tpl1{1, 3.14, "text"}; // std::tuple<int, double, const char*>

        std::pair p1{10, "text"}; // std::pair<int, const char*>
        std::tuple t1{p1};        // std::tuple<int, const char*>
    }

    SECTION("optional")
    {
        std::optional opt1 = 42;   // std::optional<int>

        std::optional opt2 = opt1; // std::optional<int> - copy rule
    }

    SECTION("smart pointers")
    {
        std::unique_ptr<int> uptr{new int(13)}; // CTAD is disabled
        std::shared_ptr<int> sptr{new int(13)}; // CTAD is disabled

        std::unique_ptr uptr_other = std::make_unique<int>(13);
        std::shared_ptr sptr_other = std::move(uptr_other);
        std::weak_ptr wptr = sptr_other;
    }

    SECTION("function")
    {
        std::function f = foobar; // std::funtion<int(int)>
        REQUIRE(f(1) == 42);
    }

    SECTION("containers")
    {
        std::vector vec{1, 2, 3, 4}; // std::vector<int>
        std::list lst{1, 2, 3, 4}; // std::list<int>

        std::vector backup_vec(lst.begin(), lst.end()); // std::vector<int>

        std::array arr{1, 2, 3, 4, 5}; // std::array<int, 5>
    }
}