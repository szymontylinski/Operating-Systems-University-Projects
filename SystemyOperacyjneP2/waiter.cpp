#include "waiter.hpp"

void Waiter::requestToEat(int) {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&] { return eating < n - 1; });
    eating++;
}

void Waiter::doneEating(int) {
    std::unique_lock<std::mutex> lock(m);
    eating--;
    cv.notify_all();
}
