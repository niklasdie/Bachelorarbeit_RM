//
// Created by dev on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "udp_sender.cpp"
#include "udp_receiver.cpp"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: <local ip> <destination ip>" << std::endl;
        return 1;
    }

    int port = 8080;

    shm s(20);
    s.set_data("Hello World");
    std::cout << "Data: " << s.get_data() << std::endl;

    udp_receiver server(s, argv[1], port);
    boost::thread th(boost::bind(&udp_receiver::receive, &server));
    std::cout << "Thread started" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    udp_sender client(s,argv[2], port);
    std::cout << "Start sending" << std::endl;
    for (int i = 0; i < 21; i++) {
        std::cout  << " \033[0;34mSM: \033[0m" << s.get_data() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        s.set_data(new char('0' + (i % 10)), i);
        client.send_data(0);
    }

    for (int i = 0; i < 1000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

//    shm s(100);
//    s.set_data("Hello World!", 10);
//    s.print_data();
//    std::cout << "get_data_of_shm(): " << s.get_data_of_shm() << std::endl;

//    pthread_cancel(th.native_handle());
    th.interrupt();
    return 0;
}