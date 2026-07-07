#ifndef WAITER_HPP
#define WAITER_HPP

#include <mutex>
#include <condition_variable>

class Waiter {
public:
    Waiter(int n) : n(n) {}

    void requestToEat(int);
    void doneEating(int);

private:
    int n;
    int eating = 0;

    std::mutex m;
    std::condition_variable cv;
};

#endif
