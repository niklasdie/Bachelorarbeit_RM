//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "shm.hpp"
#include "udp_sender.cpp"
#include "shm.cpp"
#include "timer.cpp"

#define IPADDRESS "10.0.0.1" // "127.0.0.1"
#define UDP_PORT 8080

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver {
    udp_receiver(shm &shm, const char *local_ip, int port, udp_sender &sender, timer &ti)
            : shm_(shm), host_ip(local_ip), sender(sender), ti(ti)
    {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string(host_ip), port));
    }

    ~udp_receiver()
    {
        socket.close();
    }

private:
    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        std::string message = std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
        std::cout << "\033[1;31mReceived: \033[0m" << message << "\n";
//        std::cout << "\033[1;31mReceived " << bytes_transferred << " bytes\033[0m\n";

        shm_.set_data(message);

        ti.end();

        wait();
    }

    void handle_receive_and_send_back(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        std::string message = std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
        std::cout << "\033[1;31mReceived: \033[0m" << message << "\n";
//        std::cout << "\033[1;31mReceived " << bytes_transferred << " bytes\033[0m\n";

        shm_.set_data(message);

        sender.send_data();

        wait_and_send_back();
    }

    void wait()
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

    void wait_and_send_back()
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
    void receive()
    {
        wait();

        io_service.run();
    }

    void receive_and_send_back()
    {
        wait_and_send_back();

        io_service.run();
    }

    void interrupt()
    {
        socket.close();
    }

private:
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::array<char, 1024> recv_buffer;
    shm &shm_;
    std::string host_ip;
    udp_sender &sender;
    timer &ti;
};
