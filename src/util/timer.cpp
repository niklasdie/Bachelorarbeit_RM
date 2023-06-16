//
// Created by Niklas Diekhöner on 31.03.23.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <numeric>
#include <boost/log/trivial.hpp>

struct timer
{
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> start_;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> end_;
    std::vector<long> average_;
    std::vector<long> checkpoint_;
    std::vector<int> checkpoint_size;
    std::vector<std::string> messages;
    std::ofstream file;
    std::ofstream file_average;

    timer(const char* filepath)
    {
        file.open(filepath);
        std::string s (filepath);
        file_average.open(s + "_averages");
    }

    ~timer()
    {
        checkpoint();
        std::vector<long>::iterator newIter = std::remove_if(checkpoint_.begin(), checkpoint_.end(), [](long l) { return l == 0; });
        checkpoint_.resize(newIter - checkpoint_.begin());
        int i = 1;
        for (long c : checkpoint_) {
            BOOST_LOG_TRIVIAL(info) << "\033[1;33mAverage time at " << i++
            << ". checkpoint with " << checkpoint_size.at(i - 2) << " times: "
                << c
                << "[µs]\033[0m";
            file_average << c << "\n";
        }
        BOOST_LOG_TRIVIAL(info) << "\033[1;33mAll time average: "
                                << std::accumulate(
                                        checkpoint_.begin(),
                                        checkpoint_.end(),
                                        0)
                                        / checkpoint_.size()
                                << "[µs]\033[0m";
        file.close();
        file_average.close();
    };

    void start()
    {
        start_.push_back(std::chrono::high_resolution_clock::now());
    }

    void end()
    {
        end_.push_back(std::chrono::high_resolution_clock::now());
        if (start_.size() > 0) {
            if (start_.size() >= end_.size()) {
                average_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                        end_.at(end_.size() - 1) - start_.at(end_.size() - 1)
                ).count());
            } else {
                average_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                        start_.at(start_.size() - 1) - end_.at(start_.size() - 1)
                ).count());
            }
            BOOST_LOG_TRIVIAL(info) << "Time: " << average_.at(average_.size() - 1) << "[µs]";
        }
    }

    void checkpoint()
    {
        checkpoint_.push_back(std::accumulate(average_.begin(), average_.end(), 0) / average_.size());
        checkpoint_size.push_back(average_.size());
        for (long t : average_) {
            file << t << "\n";
        }
        BOOST_LOG_TRIVIAL(debug) << "\nPackets send:     " << start_.size()
                                 << "\nPackets received: " << end_.size();
        start_.clear();
        end_.clear();
        average_.clear();
    }

    void clear()
    {
        start_.clear();
        end_.clear();
        average_.clear();
        checkpoint_.clear();
        checkpoint_size.clear();
    }
};