#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
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

TEST_CASE("string_view")
{
    auto text = "Hello string_view!"sv;

    std::string_view empty_sv;
    CHECK(empty_sv.data() == nullptr);

    std::string empty_str;
    CHECK(empty_str.data() != nullptr);

    std::string str = "ABCDEDFGH";

    std::string_view token(str.data(), 3);
    CHECK(token == "ABC");

    std::array<char, 3> raw_text = {'a', 'b', 'c'};
    std::string_view text_sv(raw_text.data(), raw_text.size());
    CHECK(text_sv == "abc");

    SECTION("conversions")
    {
        std::string text = "text";
        std::string_view text_sv = text;
        std::string_view header_sv = "HEADER";

        CHECK(text_sv == text);

        std::string header_str(header_sv);  // implicit conversion not allowed
    }
}

std::string_view start_from_word(std::string_view text, std::string_view word)
{
      return text.substr(text.find(word));
}

std::string get_line()
{
    return "HellogdgdhfdhfdhfdhfdWorld!!!";
}

TEST_CASE("string_view - why?")
{
    std::vector vec = {1, 2, 3, 4};

    std::string name = "vec";
    print_all(vec, name);

    auto prefix = "World"sv;
    auto text = "HelloWorld!!!";

    auto token = start_from_word(text, prefix);

    std::cout << token << "\n";

    auto token_alt = start_from_word(get_line(), prefix); // disaster - returned string_view refers to deallocated memory
    std::cout << token_alt << "\n";

    const std::vector<std::string> words = {"one", "two", "three", "four"};

    std::vector<std::string_view> tokens(words.begin(), words.end());
    std::sort(tokens.begin(), tokens.end());

    print_all(tokens, "tokens");
}