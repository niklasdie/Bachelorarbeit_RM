//
// Created by Niklas Diekhöner on 31.03.23.
//

#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>


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
            std::cout << "Average time: "
                      << std::accumulate(average.begin(), average.end(), 0) / average.size()
                      << "[µs]" << std::endl;
        }
    };

    void start(std::string msg)
    {
        messages.push_back(msg);
        start_.push_back(std::chrono::high_resolution_clock::now());
    }

    void end(std::string msg)
    {
        end_.push_back(std::chrono::high_resolution_clock::now());
        // TODO: Füge die Nachricht an richtiger Stelle ein
        if (start_.size() > 0) {
            std::cout << start_.size() << std::endl << end_.size() << std::endl;
            average.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                    end_.at(end_.size() - 1) - start_.at(start_.size() - 1)
            ).count());
            std::cout << "Time: " << average.at(average.size() - 1) << "[µs]" << std::endl;
        }
    }

    void clear()
    {
        start_ = std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>();
        end_ = std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>();
    }
};