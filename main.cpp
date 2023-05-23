//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "udp_receiver.cpp"
#include "shm.cpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <local ip> <destination ip>\n";
        return 1;
    }

    int port = 8080;

    // data size
    int data_size = 100;
    // shm name
    const char* shm_name = "rm_shm";

    // shm_file
    shm_o s(data_size, shm_name);

    // file mapping
//    shm_f s("shm_file.txt", data_size);

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
    client.send_data(0);

    // wait if receive package
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    if (ti.end_.size() == 0) { // Send Mode
        ti.clear();
        std::cout << "\033[1;42mSend Mode\033[0m\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Start sending\n";
        for (int i = 0; i < 1000; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ti.start(s.get_data());
            s.set_data(new char('0' + (i % 10)), i);
            client.send_data(0);
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

    std::cout << data_size << " chars and messages tested.\n"
    << "One message contains " << s.get_data().size() * sizeof(char) << " bytes.\n";
    
    return 0;
}