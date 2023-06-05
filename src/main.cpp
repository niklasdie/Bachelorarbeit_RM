//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "application_simulator/application_simulator.cpp"

// api
#include "api/shm_api.cpp"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: <local ip> <port> <shm_name>\n";
        return 1;
    }

    {
        const char* local_ip = argv[1];

        char local_ip_bytes[4];
        std::istringstream ip(local_ip);
        std::string tmp;
        for (size_t i = 0; i < sizeof(local_ip_bytes); ++i) {
            std::getline(ip, tmp, '.');
            local_ip_bytes[i] = (char) stoi(tmp);
        }

        const char* multicast_ip = "224.0.0.1";

        int port = std::stoi(argv[2]);

        // shm name
        const char* shm_name = argv[3];

        // shm
        shm_o shm(shm_name);

        // timer
        timer ti{};

        // UDP
        boost::asio::io_service io_service;
        udp_sender sender(io_service, shm, local_ip_bytes, multicast_ip, port);
        udp_receiver receiver(io_service, shm, local_ip_bytes, multicast_ip, port, sender, ti, false);
        std::cout << "Thread started" << std::endl;

        // api
        set_udp_sender(&sender);

        // simulated application
        application_simulator simulator(shm_name);

        // send first package
        sender.send_data();

        // wait if receive package
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        if (ti.end_.size() == 0) { // Send Mode
            ti.clear();
            std::cout << "\033[1;42mSend Mode\033[0m\n";

            // wait for Receive Mode to set up
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "Start sending\n";

            for (int i = 0; i < 10; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                simulator.do_something();
//                std::cout << "\t\033[1;32mData simulator: \033[0m" << *simulator.shm_s << "\n";
//                std::cout << "\t\033[1;32mData shm:       \033[0m" << s.get_data_struct() << "\n";
                ti.start();
//                rm_in((void *) &s.get_data_struct().data, sizeof(char[12]));
                rm_in();
//                client.send_data((void *) &s.get_data_struct().data, sizeof(char[12]));
//                client.send_data();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else { // Receive Mode
            ti.clear();
            std::cout << "\033[1;42mReceive Mode\033[0m\n";

            // change to receive and resend all packages
            receiver.change_to_receive_and_resend();
            std::cout << "Changed to receive and resend\n";

            // print current state of shm before all packages
            std::cout << "\t\033[1;32mData simulator before: \033[0m" << *simulator.shm_s << "\n";
            std::cout << "\t\033[1;32mData shm before:       \033[0m" << shm.get_data_struct() << "\n";

            // wait for all packages to arrive
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));

            // print current state of shm after all packages
            std::cout << "\t\033[1;31mData simulator after: \033[0m" << *simulator.shm_s << "\n";
            std::cout << "\t\033[1;31mData shm after:       \033[0m" << shm.get_data_struct() << "\n";
        }

        // print final state of shm
        std::cout << "\n\033[1;33mData shm end: " << shm.get_data_struct() << "\033[0m\n";

    }

    return 0;
}