#ifndef PHILOSOPHER_HPP
#define PHILOSOPHER_HPP

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iostream>

class Waiter;

class Philosopher {
public:
    Philosopher(int id, std::mutex& left, std::mutex& right, Waiter* waiter = nullptr);

    void startNaive();
    void startAsymmetric();
    void startWithWaiter();
    void startStarvation(); // nowy: sprawdza oba widelce i zajmuje je tylko gdy oba wolne

    void join();

    int getMeals() const { return meals; }

    long long getTotalWait() const { return totalWait; }
    long long getMaxWait() const { return maxWait; }

private:
    void think();
    void eat();

    int id;
    std::mutex& leftFork;
    std::mutex& rightFork;
    Waiter* waiter;

    std::atomic<int> meals{0};
    std::atomic<long long> totalWait{0}; // w mikrosekundach
    std::atomic<long long> maxWait{0};   // w mikrosekundach

    std::thread th;
    bool stop = false;
};

#endif
