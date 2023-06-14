//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "../api/rm_api_private.hpp"

int main(int argc, char *argv[])
{
    // check count of program arguments
    if (!(argc != 4 | argc != 6)) {
        std::cerr << "Usage: <local ip> <multicast ip> <port> <shm_name> (-log [trace, debug, info, warning, error])\n";
        return 1;
    }

    {
        // logger
//        boost::log::add_file_log
//        (
//                boost::log::keywords::file_name = "log.log"
////                boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
//        );
//        boost::log::add_console_log
//        (
//                std::clog
////                boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
//        );
        if (argc > 5 && strcmp(argv[5], "-log") == 0) {
            if (strcmp(argv[6], "trace") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::trace
                        );
            } else if (strcmp(argv[6], "debug") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::debug
                        );
            } else if (strcmp(argv[6], "info") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::info
                        );
            } else if (strcmp(argv[6], "warning") == 0) {
                boost::log::core::get()->set_filter
                        (
                                boost::log::trivial::severity >= boost::log::trivial::warning
                        );
            } else if (strcmp(argv[6], "error") == 0) {
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
        shm_o *shm = new shm_o(shm_name, sizeof(udp_sender));
        shm_o *shm_new = (shm_o *) shm->get_shm_o_address();
        std::memcpy(shm_new, shm, sizeof(*shm));
        shm = (shm_o*) shm_new;

        // timer
        timer ti{};

        // UDP
        boost::asio::io_service io_service;
        udp_sender *sender = new (shm) udp_sender(io_service, shm, multicast_ip, port, ti);
        udp_receiver receiver(io_service, shm, multicast_ip, port, sender, ti, false);

        BOOST_LOG_TRIVIAL(debug) << sizeof(udp_sender) << "; " << sizeof(*sender);
        BOOST_LOG_TRIVIAL(debug) << sizeof(shm_o) << "; " << sizeof(*shm);

        // api
//        rm_api api(shm->get_address(), shm->get_shm_size(), sizeof(shm_struct));

        while(true) {}

    }

    return 0;
}