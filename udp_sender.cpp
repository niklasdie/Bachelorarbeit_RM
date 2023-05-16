//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "shm.hpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(shm &shm, const char *dest_ip, int port)
        : shm_(shm), socket(udp::socket(io_context))
    {
        std::string dest(dest_ip);
        std::cout << "Client started at " << dest << std::endl;
        socket.open(udp::v4());
        destination_endpoint = udp::endpoint(
            address::from_string(dest), // dest.append(":").append(std::to_string(port))
            port
        );
    }

    ~udp_sender()
    {
        socket.close();
    };

    // Create new Socket for each send?
    void send_data(std::string data)
    {
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "Sent data: " << data << std::endl;
    }

    void send_data(int offset)
    {
        std::string data = shm_.get_data(offset);
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "\033[1;32mSent data: \033[0m" << data << std::endl;
    }

    void send_data(int offset, int length)
    {
        std::string data = shm_.get_data(offset, length);
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "\033[1;32mSent data: \033[0m" << data << std::endl;
    }

private:
    boost::asio::io_context io_context;
    udp::socket socket;
    udp::endpoint destination_endpoint;
    shm &shm_;
};
