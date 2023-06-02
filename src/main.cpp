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
    if (argc != 3) {
        std::cerr << "Usage: <local ip> <destination ip>\n";
        return 1;
    }

    {
        int port = 8080;

        // shm name
        const char* shm_name = "rm_shm";

        // shm
        shm_o s(shm_name);

        // timer
        timer ti{};

        // UDP
        udp_sender client(s, argv[2], port);
        udp_receiver server(s, argv[1], port, client, ti, false);
        std::cout << "Thread started" << std::endl;

        // api
        set_udp_sender(&client);

        // simulated application
        application_simulator simulator(shm_name);

        // send first package
        client.send_data();

        // wait if receive package
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        if (ti.end_.size() == 0) { // Send Mode
            ti.clear();
            std::cout << "\033[1;42mSend Mode\033[0m\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
            std::cout << "Old thread stopped\n";
            server.interrupt();
            udp_receiver server2(s, argv[1], port, client, ti, true);
            std::cout << "Thread started\n";
            std::cout << "\t\033[1;32mData simulator before: \033[0m" << *simulator.shm_s << "\n";
            std::cout << "\t\033[1;32mData shm before:       \033[0m" << s.get_data_struct() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            std::cout << "\t\033[1;31mData simulator after: \033[0m" << *simulator.shm_s << "\n";
            std::cout << "\t\033[1;31mData shm after:       \033[0m" << s.get_data_struct() << "\n";
        }

        std::cout << "\n\033[1;33mData shm end: " << s.get_data_struct() << "\033[0m\n";

    }

    return 0;
}