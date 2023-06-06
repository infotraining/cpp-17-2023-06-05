#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <array>

using namespace std;

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 543, 235, 534, 665, 654, 3456};

    if (auto pos = std::find(begin(vec), end(vec), 665); pos != end(vec))
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else
    {
        std::cout << "Item not found!\n";
        assert(pos == end(vec));
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk{mtx_q_msg};
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::unique_lock lk{mtx_q_msg}; !std::empty(q_msg))
        {
            msg = q_msg.front();
            q_msg.pop();
        }
        else
        {
            lk.unlock();
            // logger.log();
        }
    }
}

///////////////////////////////////////
// scoped locks

class BankAccount
{
    const int id_;
    double balance_;
    mutable std::mutex mtx_balance_;

public:
    BankAccount(int id, double balance)
        : id_(id)
        , balance_(balance)
    {
    }

    void print() const
    {
        std::cout << "Bank Account #" << id_ << "; Balance = " << balance() << std::endl;
    }

    void transfer(BankAccount& to, double amount)
    {
        // std::unique_lock lk_from{mtx_balance_, std::defer_lock};
        // std::unique_lock lk_to{to.mtx_balance_, std::defer_lock};

        // std::lock(lk_from, lk_to); // deadlock-free lock

        // since C++17
        std::scoped_lock lks{mtx_balance_, to.mtx_balance_};

        balance_ -= amount;
        to.balance_ += amount;
    }

    void withdraw(double amount)
    {
        std::lock_guard lk{mtx_balance_};
        balance_ -= amount;
    }

    void deposit(double amount)
    {
        std::scoped_lock lk{mtx_balance_};
        balance_ += amount;
    }

    int id() const
    {
        return id_;
    }

    double balance() const
    {
        std::lock_guard lk{mtx_balance_};
        return balance_;
    }
};

void make_withdraws(BankAccount& ba, int no_of_operations)
{
    for (int i = 0; i < no_of_operations; ++i)
        ba.withdraw(1.0);
}

void make_deposits(BankAccount& ba, int no_of_operations)
{
    for (int i = 0; i < no_of_operations; ++i)
        ba.deposit(1.0);
}

void make_transfers(BankAccount& from, BankAccount& to, int no_of_operations, int thd_id)
{
    for (int i = 0; i < no_of_operations; ++i)
    {
        from.transfer(to, 1.0);
    }
}

// TEST_CASE("locks")
// {
//     constexpr int NO_OF_ITERS = 1'000'000;

//     BankAccount ba1(1, 10'000);
//     BankAccount ba2(2, 10'000);

//     std::thread thd1(&make_withdraws, std::ref(ba1), NO_OF_ITERS);
//     std::thread thd2(&make_deposits, std::ref(ba1), NO_OF_ITERS);

//     thd1.join();
//     thd2.join();

//     std::cout << "After threads: ";
//     ba1.print();

//     std::cout << "\nTransfer:" << std::endl;

//     std::thread thd3(&make_transfers, std::ref(ba1), std::ref(ba2), NO_OF_ITERS, 1);
//     std::thread thd4(&make_transfers, std::ref(ba2), std::ref(ba1), NO_OF_ITERS, 2);

//     thd3.join();
//     thd4.join();

//     ba1.print();
//     ba2.print();
// }

/////////////////////////////////////////////////////////////////////////////////////////////////
// constexpr if

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {

        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

template <typename T>
auto is_power_of_2(T value)
{
    if constexpr(std::is_integral_v<T>)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

auto make_buffer()
{
    if constexpr(sizeof(int) > 4)
    {
        return std::vector<int>(100);
    }
    else
    {
        return std::array<int, 100>{};
    }
}

TEST_CASE("constexpr if")
{
    CHECK(is_power_of_2(8));
    CHECK(is_power_of_2(32));
    CHECK(is_power_of_2(49) == false);

    CHECK(is_power_of_2(8.0));

    auto buffer = make_buffer();
    CHECK(buffer.size() == 100);

    std::cout << typeid(buffer).name() << "\n";
}
