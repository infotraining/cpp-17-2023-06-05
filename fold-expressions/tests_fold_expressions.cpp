#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////
// NTTP (Non-Type Template Params with auto)

template <typename T, size_t N>
struct Array
{
    T items[N];

    using iterator = T*;
    using const_iterator = const T*;
    using reference = T&;

    constexpr iterator begin()
    {
        return items;
    }

    constexpr iterator end()
    {
        return items + N;
    }

    constexpr const_iterator begin() const
    {
        return items;
    }

    constexpr const_iterator end() const
    {
        return items + N;
    }

    constexpr size_t size() const
    {
        return N;
    }
};

// deduction guide + variadic template
template <typename T, typename... Ts>
Array(T, Ts...) -> Array<T, sizeof...(Ts) + 1>;

template <auto N>
struct Value
{
    static inline constexpr auto value = N;
};

template <auto... Ns>
struct ValueList
{
};

TEST_CASE("Array")
{
    Array<int, 10> arr1 = {1, 2, 3, 4};
    Array arr2 = {1, 2, 3, 4, 5};

    static_assert(Value<'c'>::value == 'c');

    ValueList<1, 2, 42u, nullptr, -3> vl;
}

////////////////////////////////////////////////////////////////////////////////
// variadic templates

template <typename... Ts> // parameter pack
struct Tuple
{
    static constexpr size_t tuple_size = sizeof...(Ts);

    std::tuple<Ts...> value; // parameter pack expansion
};

struct Person
{
    std::string first_name;
    std::string last_name;
    int age = -1;

    Person() = default;

    Person(std::string fn, std::string ln)
        : first_name{fn}
        , last_name{ln}
    {
    }

    Person(std::string fn, std::string ln, int a)
        : first_name{fn}
        , last_name{ln}
        , age{a}
    {
    }
};

template <typename T, typename... TArgs>
std::unique_ptr<T> my_make_unique(TArgs&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
}

TEST_CASE("variadic template")
{
    Tuple<> t0;
    static_assert(Tuple<>::tuple_size == 0);
    Tuple<int> t1;
    Tuple<int, double> t2;
    Tuple<int, int, int, double, std::string> t3{{1, 2, 3, 3.14, "text"}};
    static_assert(decltype(t3)::tuple_size == 5);

    std::unique_ptr<Person> p1 = my_make_unique<Person>("Jan", "Kowalski");
    std::unique_ptr<Person> p2 = my_make_unique<Person>();
}

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item)
            {
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

    //////////////////////////////////////////

    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(THead head, TTail... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
} // namespace BeforeCpp17

namespace Cpp17
{
    namespace ConstexprImpl
    {
        template <typename THead, typename... TTail>
        void print(const THead& head, const TTail&... tail)
        {
            std::cout << head << " ";

            if constexpr (sizeof...(tail) > 0)
                print(tail...);
            else
                std::cout << "\n";
        }
    }

    inline namespace FoldImpl
    {
        template <typename... TArgs>
        void print(const TArgs&... args)
        {
            auto with_space = [is_first = true](const auto& item) mutable
            {
                if (!is_first)
                    std::cout << " ";
                is_first = false;
                return item;
            };

            (std::cout << ... << with_space(args)) << "\n"; // binary left fold
        }
    }

    template <typename... TArgs>
    void print_lines(const TArgs&... args)
    {
        (..., (std::cout << args << "\n")); // fold with , operator
        // std::cout << arg1 << "\n";
        // std::cout << arg2 << "\n";
        // std::cout << arg3 << "\n";
    }

    template <typename... TArgs>
    auto sum(const TArgs&... args)  // 1, 2, 3, 4
    {
        return (... + args); // left-fold: (((1 + 2) + 3) + 4)
    }

    template <typename... TArgs>
    auto sum_r(const TArgs&... args) // 1, 2, 3, 4
    {
        return (args + ...);  // right-fold: (1 + (2 + (3 + 4))))
    }

    template <typename F, typename... TArgs>
    decltype(auto) apply_function(F f, TArgs... args)
    {
        return (..., f(args));
    } 
}

TEST_CASE("variadic sum")
{
    using Cpp17::print;
    using Cpp17::sum;

    CHECK(sum(1, 2, 3, 4) == 10);
    CHECK(sum(1, 2, 3, 4, 5) == 15);

    print(1, "text", 3.14);
    // print(head = 1, tail = ("text", 3.14))
    //   print(head = "text", tail = (3.14))
    //     print(head =3.14, tail = ())
    //       print()

    Cpp17::print_lines(1, "text", 3.14);

    auto printer = [](const auto& v) { std::cout << "value: " << v << "\n"; };
    Cpp17::apply_function(printer, 1, 2, "Text", 3.14);
}