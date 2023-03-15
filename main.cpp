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
    if (argc != 2) {
        std::cerr << "Usage: client <host>" << std::endl;
        return 1;
    }
    int port = 8080;

    shm s(20);
    s.set_data("Hello World");
    std::cout << "Data: " << s.get_data() << std::endl;

    udp_receiver server(s, port);
    boost::thread th(boost::bind(&udp_receiver::receive, &server));
    std::cout << "Thread started" << std::endl;

    udp_sender client(s,argv[1], port);
    std::cout << "Start sending" << std::endl;
    for (int i = 0; i < 20; i++) {
        client.send_data(0, 11);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "SM: " << s.get_data() << std::endl;
        s.set_data("!", i);
        std::cout << "Changed data" << std::endl;
        std::cout << "SM: " << s.get_data() << std::endl;
    }

//    shm s(100);
//    s.set_data("Hello World!", 10);
//    s.print_data();
//    std::cout << "get_data_of_shm(): " << s.get_data_of_shm() << std::endl;

//    pthread_cancel(th.native_handle());
    th.interrupt();
    return 0;
}