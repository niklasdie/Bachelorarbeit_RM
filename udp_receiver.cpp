//
// Created by dev on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "shm.hpp"

#define IPADDRESS "10.0.0.1" // "127.0.0.1"
#define UDP_PORT 8080

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver
{
    udp_receiver(shm &shm, const char *host, int port)
        : shm_(shm), host_ip(host), port(port)
    {
    }

    ~udp_receiver() = default;

private:
    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        std::string message = std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
        std::cout << "\033[1;31mReceived: \033[0m" << message << std::endl;

        shm_.set_data(message);

        wait();
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

public:
    void receive()
    {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string(host_ip), port));

        wait();

        io_service.run();
    }

private:
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::array<char, 1024> recv_buffer;
//    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(IPADDRESS), UDP_PORT);
    shm &shm_;
    std::string host_ip;
    int port;
};
