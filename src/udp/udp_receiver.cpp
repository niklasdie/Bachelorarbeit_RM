//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#include "udp_payload.hpp"
#include "udp_sender.cpp"
#include "../shm/shm.cpp"
#include "../util/timer.cpp"

#define IPADDRESS "10.0.0.1" // "127.0.0.1"
#define UDP_PORT 8080

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver {
    udp_receiver(shm &shm, const char *local_ip, int port, udp_sender &sender, timer &ti, bool resend)
            : shm_(shm), host_ip(local_ip), sender(sender), ti(ti)
    {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string(host_ip), port));
        socket.set_option(boost::asio::socket_base::receive_buffer_size(1024));
        th = boost::thread(boost::bind(&udp_receiver::run_service, this));
        if(resend) {
            start_receive_and_send_back();
        } else {
            start_receive();
        }
    }

    ~udp_receiver()
    {
        th.interrupt();
        socket.close();
    }

private:

    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }

        std::cout << "\t\033[1;41mReceived:\033[0m\n";
        std::cout << "\t\033[1;31mBytes:        \033[0m" << bytes_transferred << "\n";

//        std::vector<udp_buffer> buffer = *(std::vector<udp_buffer> *) &recv_buffer;
//        for (udp_buffer package : buffer) {
//            shm_.set_data(&package.data, package.offset, package.length);
//        }

        udp_payload package = *(udp_payload *) &recv_buffer;
//        std::cout << "\t\033[1;31mPackage data: \033[0m" << package << "\n";

        shm_.set_data(&package.data, package.offset, package.length);

        std::cout << "\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct() << "\n";

        ti.end();

        start_receive();
    }

    void handle_receive_and_send_back(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }

        std::cout << "\t\033[1;41mReceived:\033[0m\n";
        std::cout << "\t\033[1;31mBytes:        \033[0m" << bytes_transferred << "\n";

//        std::vector<udp_buffer> buffer = *(std::vector<udp_buffer> *) &recv_buffer;
//        for (udp_buffer package : buffer) {
//            shm_.set_data(&package.data, package.offset, package.length);
//        }

        udp_payload package = *(udp_payload *) &recv_buffer;
//        std::cout << "\t\033[1;31mPackage data: \033[0m" << package << "\n";
//        std::cout << "\t\033[1;31mINT data: \033[0m" << *(int*)&package.data << "\n";

        shm_.set_data(&package.data, package.offset, package.length);

        std::cout << "\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct() << "\n";

//        shm_.set_data(&recv_buffer, bytes_transferred);

//        sender.send_data((void *) &shm_.get_data_struct().data, sizeof(char[12]));
        sender.send_data();

        start_receive_and_send_back();
    }

    void start_receive()
    {
        socket.async_receive(boost::asio::buffer(recv_buffer),
                             boost::bind(
                                     &udp_receiver::handle_receive,
                                     this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred
                             )
        );
    }

    void start_receive_and_send_back()
    {
        socket.async_receive(boost::asio::buffer(recv_buffer),
                             boost::bind(
                                     &udp_receiver::handle_receive_and_send_back,
                                     this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred
                             )
        );
    }

public:
    void run_service()
    {
        io_service.run();
    }

    void interrupt()
    {
        th.interrupt();
        socket.close();
    }

private:
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::thread th;
//    boost::asio::strand strand;
    shm &shm_;
    char recv_buffer[1460];
    std::string host_ip;
    udp_sender &sender;
    timer &ti;
};
