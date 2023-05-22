//
// Created by dev on 22.05.23.
//

#include <chrono>
#include <thread>
#include "application_simulator.hpp"

int main() {
    application_simulator as;

    for (int i = 0; i < 100; i++) {
        as.do_something();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << i << "\n";
    }
}