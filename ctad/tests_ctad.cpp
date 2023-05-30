#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];
}