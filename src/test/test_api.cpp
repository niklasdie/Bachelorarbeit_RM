//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "../application_simulator/application_simulator.cpp"

// api
#include "../api/rm_api_private.hpp"

int main(int argc, char *argv[])
{
    // check count of program arguments
    if (!(argc != 4 | argc != 6)) {
        std::cerr << "Usage: <local ip> <multicast ip> <port> <shm_name> <time_file> (-log [trace, debug, info, warning, error])\n";
        return 1;
    }

    {
        // logger
        boost::log::add_common_attributes();
        boost::log::add_file_log
                (
                        boost::log::keywords::file_name = "log%N.log",
                        boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
                );
        boost::log::add_console_log
                (
                        std::cout,
                        boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
                );
        if (argc > 5 && strcmp(argv[6], "-log") == 0) {
            if (strcmp(argv[7], "trace") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::trace
                        );
            } else if (strcmp(argv[7], "debug") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::debug
                        );
            } else if (strcmp(argv[7], "info") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::info
                        );
            } else if (strcmp(argv[7], "warning") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::warning
                        );
            } else if (strcmp(argv[7], "error") == 0) {
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

        // local ip to broadcast ip
        const char* multicast_ip = argv[2];

        // port
        int port = std::stoi(argv[3]);

        // shm name
        const char* shm_name = argv[4];

        // shm
        shm_o shm(shm_name);

        // timer
        timer ti(argv[5]);

        // UDP
        boost::asio::io_service io_service;
        udp_sender sender(io_service, shm, multicast_ip, port, ti);
        udp_receiver receiver(io_service, shm, multicast_ip, port, sender, ti, false);

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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            BOOST_LOG_TRIVIAL(debug) << "Start sending";


            /// api test
            char data[12] = "Hallo Test";
            BOOST_LOG_TRIVIAL(debug) << "sync_all_rm()";
            sync_all_rm();
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_s(&shm.get_data_struct())";
            rm_in_s(&shm.get_data_struct());
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_sl(&shm.get_data_struct(), 10)";
            rm_in_sl(&shm.get_data_struct(), 10);
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_ts<int>(&shm.get_data_struct().data)";
            rm_in_ts<char[12]>(&shm.get_data_struct().data);
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_sdl(data, &shm.get_data_struct().data, sizeof(data))";
            rm_in_sdl(data,  &shm.get_data_struct().data, sizeof(data));
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_tsd<char[12]>(data, &shm.get_data_struct().data)";
            rm_in_tsd<char[12]>(data,  &shm.get_data_struct().data);
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_sol(data, 0, sizeof(data))";
            rm_in_sol(data, 0, sizeof(data));
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_tso<char[12]>(data, 0)";
            rm_in_tso<char[12]>(data, 0);
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_vd(&shm.get_data_struct().i, 10)";
            rm_in_vd(&shm.get_data_struct().i, 10); // problem with c_str
            simulator.do_something();
            BOOST_LOG_TRIVIAL(debug) << "rm_in_vo(12, 12)";
            rm_in_vo(12, 12); // problem with c_str


            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        } else { // Receive Mode
            ti.clear();
            BOOST_LOG_TRIVIAL(info) << "\033[1;42mReceive Mode\033[0m";

            // print current state of shm before all packages
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;32mData simulator before: \033[0m" << *simulator.shm_s;
            BOOST_LOG_TRIVIAL(debug) << "\t\033[1;32mData shm before:       \033[0m" << shm.get_data_struct();

            // wait for all packages to arrive
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));

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