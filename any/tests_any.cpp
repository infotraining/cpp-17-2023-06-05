#include <algorithm>
#include <any>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;


TEST_CASE("any")
{
    std::any anything;

    CHECK(anything.has_value() == false);

    anything = 42;
    anything = 3.14;
    anything = "text"s;
    anything = std::vector{1, 3, 4};

    auto vec = std::any_cast<std::vector<int>>(anything);
    CHECK(vec == std::vector{1, 3, 4});

    CHECK_THROWS_AS(std::any_cast<int>(anything), std::bad_any_cast);

    std::cout << "Now in anything: " << anything.type().name() << "\n";


    if (auto* ptr =  std::any_cast<std::vector<int>>(&anything); ptr != nullptr)
    {
        std::cout << "vec.size(): " << ptr->size() << "\n";
    }
}

////////////////////////////////////
// wide interfaces

class Observer
{
public:
    virtual void update(std::any sender, const string& msg) = 0;
    virtual ~Observer() = default;
};

class TempMonitor
{
    std::vector<Observer*> observes_;
public:
    void notify()
    {
        for(const auto& o : observes_)
            o->update(this, std::to_string(get_temp()));
    }
    double get_temp() const
    {
        return 23.88;
    }
};

class Logger : public Observer
{
public:
    void update(std::any sender, const string& msg) override
    {
        TempMonitor* const* monitor = std::any_cast<TempMonitor*>(&sender);
        if (monitor)
            (*monitor)->get_temp();
    }
};

///////////////////////////////////////////////////////////

class DynamicDict
{
    std::unordered_map<std::string, std::any> dict_;
public:
    auto insert(std::string key, std::any value)
    {
        return dict_.emplace(std::move(key), std::move(value));
    }

    template <typename T>
    T get(const std::string& key)
    {
        return std::any_cast<T>(dict_.at(key));
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dynamic dict

TEST_CASE("dynamic dict")
{
    DynamicDict dd;

    dd.insert("id", 42);
    dd.insert("name", "John"s);

    std::cout << dd.get<int>("id") << " - " << dd.get<std::string>("name") << "\n";
}