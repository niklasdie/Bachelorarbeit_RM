//
// Created by Niklas Diekhöner on 31.03.23.
//

#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>


struct timer
{
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> start_;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> end_;
    std::vector<long> average;
    std::vector<std::string> messages;

    timer() = default;

    ~timer()
    {
        if (average.size() > 0) {
            long sum;
            for (long l: average) {
                sum += l;
            }
            std::cout << "\033[1;33mAverage time: "
                      << std::accumulate(average.begin(), average.end(), 0) / average.size()
                      << "[µs]\033[0m\n";
        }
    };

    void start()
    {
        start_.push_back(std::chrono::high_resolution_clock::now());
    }

    void end()
    {
        end_.push_back(std::chrono::high_resolution_clock::now());
        if (start_.size() > 0) {
//            std::cout << "Start times: " << start_.size() << "\nEnd times: " << end_.size() << "\n";
            average.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                    end_.at(end_.size() - 1) - start_.at(start_.size() - 1)
            ).count());
            std::cout << "Time: " << average.at(average.size() - 1) << "[µs]\n";
        }
    }

    void clear()
    {
        start_ = std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>();
        end_ = std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>();
    }
};