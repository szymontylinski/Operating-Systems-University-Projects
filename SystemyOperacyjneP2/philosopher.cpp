#include "philosopher.hpp"
#include "waiter.hpp"
#include <algorithm> // std::max
#include <thread>

Philosopher::Philosopher(int id, std::mutex& left, std::mutex& right, Waiter* waiter)
    : id(id), leftFork(left), rightFork(right), waiter(waiter) {}

void Philosopher::think() {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

void Philosopher::eat() {
    meals++;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

void Philosopher::startNaive() {
    th = std::thread([this]() {
        while (!stop) {
            think();
            auto start = std::chrono::steady_clock::now();
            std::scoped_lock lock(leftFork, rightFork);
            auto end = std::chrono::steady_clock::now();

            long long wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            totalWait += wait;
            maxWait = std::max(maxWait.load(), wait);

            eat();
        }
    });
}

void Philosopher::startAsymmetric() {
    th = std::thread([this]() {
        while (!stop) {
            think();

            auto start = std::chrono::steady_clock::now();

            if (id % 2 == 0) {
                std::scoped_lock lock(rightFork, leftFork);
                auto end = std::chrono::steady_clock::now();
                long long wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                totalWait += wait;
                maxWait = std::max(maxWait.load(), wait);
                eat();
            } else {
                std::scoped_lock lock(leftFork, rightFork);
                auto end = std::chrono::steady_clock::now();
                long long wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                totalWait += wait;
                maxWait = std::max(maxWait.load(), wait);
                eat();
            }
        }
    });
}

void Philosopher::startWithWaiter() {
    th = std::thread([this]() {
        while (!stop) {
            think();

            auto start = std::chrono::steady_clock::now();
            waiter->requestToEat(id);

            {
                std::scoped_lock lock(leftFork, rightFork);
                auto end = std::chrono::steady_clock::now();
                long long wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                totalWait += wait;
                maxWait = std::max(maxWait.load(), wait);

                eat();
            }

            waiter->doneEating(id);
        }
    });
}

// Nowa metoda: próbujemy zająć oba widelce tylko jeśli oba są wolne.
// Używa try_lock, nie blokuje — może powodować zagłodzenie.
void Philosopher::startStarvation() {
    th = std::thread([this]() {
        using namespace std::chrono;
        while (!stop) {
            think();

            auto start = steady_clock::now();
            bool ate = false;

            // próby aż do zdobycia obu widelców lub do stop
            while (!stop && !ate) {
                // najpierw spróbuj zamknąć lewy
                if (leftFork.try_lock()) {
                    // lewy zdobyty, spróbuj prawego
                    if (rightFork.try_lock()) {
                        // oba zdobyte
                        auto end = steady_clock::now();
                        long long wait = duration_cast<microseconds>(end - start).count();
                        totalWait += wait;
                        maxWait = std::max(maxWait.load(), wait);

                        eat();

                        // zwolnij widelce
                        rightFork.unlock();
                        leftFork.unlock();

                        ate = true;
                        break;
                    } else {
                        // nie udało się zdobyć prawego -> zwolnij lewy i czekaj krótko
                        leftFork.unlock();
                        std::this_thread::sleep_for(milliseconds(1));
                    }
                } else {
                    // nie udało się zdobyć lewego -> krótka przerwa (redukcja busy-wait)
                    std::this_thread::sleep_for(milliseconds(1));
                }
            }
        }
    });
}

void Philosopher::join() {
    stop = true;
    if (th.joinable()) th.join();
}
