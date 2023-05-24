//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "shm.cpp"
#include "udp_receiver.cpp"
#include "application_simulator.cpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <local ip> <destination ip>\n";
        return 1;
    }

    int port = 8080;

    // shm name
    const char* shm_name = "rm_shm";

    // shm
    shm_o s(shm_name);

//    s.set_data("Hello World");
    std::cout << "Data: " << s.get_data() << "\n";

    // Timer
    timer ti{};

    // UDP
    udp_sender client(s, argv[2], port);
    udp_receiver server(s, argv[1], port, client, ti);
    boost::thread th(boost::bind(&udp_receiver::receive, &server));
    std::cout << "Thread started" << std::endl;

    // send first package
    client.send_data();

    // wait if receive package
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    if (ti.end_.size() == 0) { // Send Mode
        ti.clear();
        application_simulator simulator(shm_name);
        std::cout << "\033[1;42mSend Mode\033[0m\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Start sending\n";
        for (int i = 0; i < 10; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ti.start();
            simulator.do_something();
//            std::cout << "\033[1;42mData simulator: \033[0m" << *simulator.shm_s << "\n";
            std::cout << "\033[1;42mData shm:       \033[0m" << s.get_data() << "\n";
            client.send_data();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        th.interrupt();
        server.interrupt();
    } else { // Receive Mode
        ti.clear();
        std::cout << "\033[1;42mReceive Mode\033[0m\n";
        std::cout << "Old thread stopped\n";
        th.interrupt();
        server.interrupt();
        udp_receiver server2(s, argv[1], port, client, ti);
        boost::thread th2(boost::bind(&udp_receiver::receive_and_send_back, &server2));
        std::cout << "Thread started\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        th2.interrupt();
        server2.interrupt();
    }

    std::cout << "Shm size: " << sizeof(s.get_data()) << " bytes.\n";
    
    return 0;
}