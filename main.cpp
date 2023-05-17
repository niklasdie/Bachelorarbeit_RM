//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "udp_receiver.cpp"
#include "shm.hpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <local ip> <destination ip>" << std::endl;
        return 1;
    }

    int port = 8080;

    // shm_file
    shm_o s(20);

    // file mapping
//    shm_f s("/home/dev/CLionProjects/proof_of_concept/shm_file.txt", 20);

//    s.set_data("Hello World");
    std::cout << "Data: " << s.get_data() << std::endl;

    // Timer
    timer ti{};

    // UDP
    udp_sender client(s, argv[2], port);
    udp_receiver server(s, argv[1], port, client, ti);
    boost::thread th(boost::bind(&udp_receiver::receive, &server));
    std::cout << "Thread started" << std::endl;

    // send first package
    client.send_data(0);

    // wait if receive package
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    if (ti.end_.size() == 0) { // Send Mode
        ti.clear();
        std::cout << "\033[1;42mSend Mode\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Start sending" << std::endl;
        for (int i = 0; i < 21; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            ti.start(s.get_data());
            s.set_data(new char('0' + (i % 10)), i);
            client.send_data(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        th.interrupt();
    } else { // Receive Mode
        ti.clear();
        std::cout << "\033[1;42mReceive Mode\033[0m" << std::endl;
        th.interrupt();
        std::cout << "Old thread stopped" << std::endl;
        boost::thread th2(boost::bind(&udp_receiver::receive_and_send_back, &server));
        std::cout << "Thread started" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        th2.interrupt();
    }

//    for (int i = 0; i < 1000; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
//    }

//    th.interrupt();
    return 0;
}