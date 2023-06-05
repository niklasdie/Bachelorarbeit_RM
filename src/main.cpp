//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "application_simulator/application_simulator.cpp"

// api
#include "api/shm_api.cpp"

int main(int argc, char *argv[])
{
    if (!(argc != 4 | argc != 6)) {
        std::cerr << "Usage: <local ip> <port> <shm_name> (-log [trace, debug, info, warning, error])\n";
        return 1;
    }
    {
        // logger
        boost::log::add_file_log
        (
                boost::log::keywords::file_name = "log.log",
                boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
        );
        boost::log::add_console_log
        (
                std::cout,
                boost::log::keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
        );
        if (argc > 5 && strcmp(argv[4], "-log") == 0) {
            if (strcmp(argv[5], "trace") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::trace
                        );
            } else if (strcmp(argv[5], "debug") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::debug
                        );
            } else if (strcmp(argv[5], "info") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::info
                        );
            } else if (strcmp(argv[5], "warning") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::warning
                        );
            } else if (strcmp(argv[5], "error") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::error
                        );
            }
        } else {
            boost::log::core::get()->set_filter
                    (
                            boost::log::trivial::severity >= boost::log::trivial::warning
                    );
        }

        // local ip to bytes
        const char* local_ip = argv[1];
        char local_ip_bytes[4];
        std::istringstream ip(local_ip);
        std::string tmp;
        for (size_t i = 0; i < sizeof(local_ip_bytes); ++i) {
            std::getline(ip, tmp, '.');
            local_ip_bytes[i] = (char) stoi(tmp);
        }

        // local ip to broadcast ip
        tmp = std::string(local_ip);
        tmp = tmp.replace(tmp.find_last_of('.') + 1, tmp.length() - tmp.find_last_of('.'), "255");
        const char* broadcast_ip = tmp.c_str();

        // port
        int port = std::stoi(argv[2]);

        // shm name
        const char* shm_name = argv[3];

        // shm
        shm_o shm(shm_name);

        // timer
        timer ti{};

        // UDP
        boost::asio::io_service io_service;
        udp_sender sender(io_service, shm, local_ip_bytes, broadcast_ip, port);
        udp_receiver receiver(io_service, shm, local_ip_bytes, broadcast_ip, port, sender, ti, false);

        // api
        set_udp_sender(&sender);
        set_shm(&shm);

        // simulated application
        application_simulator simulator(shm_name);

        // send first package
        sender.send_data();

        // wait if receive package
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        if (ti.end_.size() == 0) { // Send Mode
            ti.clear();
            BOOST_LOG_TRIVIAL(info) << "\033[1;42mSend Mode\033[0m";

            // wait for Receive Mode to set up
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            BOOST_LOG_TRIVIAL(debug) << "Start sending";

            for (int i = 0; i < 10; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                simulator.do_something();
                ti.start();
//                rm_in((void *) &s.get_data_struct().data, sizeof(char[12]));
                rm_in();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else { // Receive Mode
            ti.clear();
            BOOST_LOG_TRIVIAL(info) << "\033[1;42mReceive Mode\033[0m";

            // change to receive and resend all packages
            receiver.change_to_receive_and_resend();
            BOOST_LOG_TRIVIAL(debug) << "Changed to receive and resend";

            // print current state of shm before all packages
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;32mData simulator before: \033[0m" << *simulator.shm_s;
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;32mData shm before:       \033[0m" << shm.get_data_struct();

            // wait for all packages to arrive
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));

            // print current state of shm after all packages
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;31mData simulator after: \033[0m" << *simulator.shm_s;
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;31mData shm after:       \033[0m" << shm.get_data_struct();
        }

        // print final state of shm
        BOOST_LOG_TRIVIAL(info) << "\033[1;33mData shm end: " << shm.get_data_struct() << "\033[0m";

    }

    stop_rm_daemon();
    return 0;
}