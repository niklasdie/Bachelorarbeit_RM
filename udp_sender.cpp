//
// Created by dev on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "shm.hpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(shm& shm, const char *host, int port) : shm_(shm)
    {
        std::string h(host);
//        udp::resolver resolver(io_context);
        std::cout << "Client started" << std::endl;
        std::cout << h << std::endl;
        destination_endpoint = udp::endpoint(
            address::from_string(h), // h.append(":").append(std::to_string(port))
            port
        );
    }

    ~udp_sender() = default;

    void send_data(std::string data)
    {
        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "Sent data: " << data << std::endl;
        socket.close();
    }

    void send_data(int offset)
    {
        std::string data = shm_.get_data_of_shm(offset);
        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "\033[1;32mSent data: \033[0m" << data << std::endl;
        socket.close();
    }

    void send_data(int offset, int length)
    {
        std::string data = shm_.get_data_of_shm(offset, length);
        udp::socket socket(io_context);
        socket.open(udp::v4());
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "\033[1;32mSent data: \033[0m" << data << std::endl;
        socket.close();
    }

private:
    boost::asio::io_context io_context;
    udp::endpoint destination_endpoint;
    shm& shm_;
};
