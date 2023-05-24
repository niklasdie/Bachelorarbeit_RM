//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "shm.cpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(shm &shm, const char *dest_ip, int port)
        : shm_(shm), socket(udp::socket(io_context))
    {
        std::string dest(dest_ip);
        std::cout << "Client started at " << dest << "\n";
        socket.open(udp::v4());
        destination_endpoint = udp::endpoint(
            address::from_string(dest), // dest.append(":").append(std::to_string(port))
            port
        );
    }

    ~udp_sender()
    {
        socket.close();
    }

    void send_data()
    {
        std::string data = shm_.get_data();
        socket.send_to(boost::asio::buffer(data), destination_endpoint);
        std::cout << "\033[1;32mSent data: \033[0m" << data << "\n";
//        std::cout << "\033[1;32mSent data\033[0m\n";
    }

private:
    boost::asio::io_context io_context;
    udp::socket socket;
    udp::endpoint destination_endpoint;
    shm &shm_;
};
