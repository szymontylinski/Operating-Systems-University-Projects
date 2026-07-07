#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
#include "philosopher.hpp"
#include "waiter.hpp"

// Funkcja pomocnicza – uruchamia jedną symulację i wypisuje wyniki
void runSimulation(int strategy, int N, int SIM_TIME_MS) {
    std::vector<std::mutex> forks(N);
    Waiter waiter(N);
    std::vector<Philosopher*> phil;

    std::cout << "\n\n=== START: Strategia " << strategy << " ===\n";

    for (int i = 0; i < N; i++) {
        Waiter* w = (strategy == 2 ? &waiter : nullptr);
        phil.push_back(new Philosopher(i, forks[i], forks[(i + 1) % N], w));
    }

    for (int i = 0; i < N; i++) {
        if (strategy == 0) phil[i]->startNaive();
        else if (strategy == 1) phil[i]->startAsymmetric();
        else if (strategy == 2) phil[i]->startWithWaiter();
        else phil[i]->startStarvation(); // strategy == 3
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(SIM_TIME_MS));

    for (auto p : phil) p->join();

    std::cout << "\n=== Wyniki po " << SIM_TIME_MS << " ms ===\n";

    long long sumMeals = 0;
    double sumAvgWait = 0;
    double sumMaxWait = 0;

    for (int i = 0; i < N; i++) {
        long long total_us = phil[i]->getTotalWait();
        long long max_us = phil[i]->getMaxWait();
        int meals = phil[i]->getMeals();

        // Konwersja na ms
        double avg_ms = meals > 0 ? (total_us / 1000.0) / meals : 0.0;
        double max_ms = max_us / 1000.0;

        sumMeals += meals;
        sumAvgWait += avg_ms;
        sumMaxWait += max_ms;

        std::cout << "Filozof " << i << ": "
                  << "posilki=" << meals
                  << ", avg_wait=" << avg_ms << " ms"
                  << ", max_wait=" << max_ms << " ms\n";
    }

    // Obliczenia globalne
    double avgMeals   = (double)sumMeals / N;
    double avgWaitAll = sumAvgWait / N;
    double avgMaxAll  = sumMaxWait / N;

    std::cout << "\n--- Podsumowanie strategii " << strategy << " ---\n";
    std::cout << "Srednia liczba posilkow: " << avgMeals << "\n";
    std::cout << "Sredni czas oczekiwania: " << avgWaitAll << " ms\n";
    std::cout << "Sredni maksymalny czas oczekiwania: " << avgMaxAll << " ms\n";

    for (auto p : phil) delete p;
}


int main() {
    int strategy;
    std::cout << "=== Wybierz strategie ===\n";
    std::cout << "0 - Naiwna \n";
    std::cout << "1 - Asymetryczna\n";
    std::cout << "2 - Kelner\n";
    std::cout << "3 - Zaglodzenie (sprawdz oba widelce - try_lock)\n";
    std::cout << "4 - Test wszystkich strategii (0,1,2,3)\n";
    std::cout << "Wybór: ";
    std::cin >> strategy;

    const int N = 11;
    const int SIM_TIME_MS = 5000;

    if (strategy == 4) {
        // Test wszystkich strategii po kolei (0..3)
        for (int s = 0; s <= 3; ++s) {
            runSimulation(s, N, SIM_TIME_MS);
        }
    } else if (strategy >= 0 && strategy <= 3) {
        // Test pojedynczej strategii
        runSimulation(strategy, N, SIM_TIME_MS);
    } else {
        std::cout << "Nieprawidlowy wybor.\n";
    }

    return 0;
}
