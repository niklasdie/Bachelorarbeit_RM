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
        socket.set_option(boost::asio::socket_base::receive_buffer_size(1024));
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
//        std::string message = shm_.get_data_bytes_as_string();
        std::cout << "\t\033[1;42mSent:\033[0m\n";
        std::cout << "\t\033[1;32mData message: \033[0m" << shm_.get_data_bytes_as_string() << "\n";
        std::cout << "\t\033[1;32mMessage size: \033[0m" << sizeof(*(shm_struct *) shm_.get_data()) << "\n";
        socket.send_to(boost::asio::buffer(
                shm_.get_data(), shm_.get_size()
                ), destination_endpoint);
//        std::cout << "\033[1;32mSent data: \033[0m" << data << "\n";
//        std::cout << "\033[1;32mSent data\033[0m\n";
    }

private:
    boost::asio::io_context io_context;
    udp::socket socket;
    udp::endpoint destination_endpoint;
    shm &shm_;
};
