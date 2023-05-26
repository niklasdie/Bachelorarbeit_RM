//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "shm_api.hpp"
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
        socket.set_option(boost::asio::socket_base::receive_buffer_size(1024));
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
//        std::cout << "\033[1;31mReceived: \033[0m" << message << "\n";
        std::cout << "\033[1;31mReceived " << bytes_transferred << " bytes; \033[0m" << error.message() << "\n";

//        shm_.set_data(recv_buffer.c_array(), bytes_transferred);

        std::cout << "\t\033[1;41mReceived:\033[0m\n";
        std::cout << "\t\033[1;32mData message: \033[0m" << shm_.get_data_bytes_as_string() << "\n";
        std::cout << "\t\033[1;32mMessage size: \033[0m" << sizeof(shm_.get_data_struct()) << "\n";

        ti.end();

        wait();
    }

    void handle_receive_and_send_back(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
//        std::string message(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
//        std::cout << "\033[1;31mReceived: \033[0m" << message << "\n";
        std::cout << "\033[1;31mReceived " << bytes_transferred << " bytes; \033[0m" << error.message() << "\n";

        shm_.set_data(recv_buffer.c_array(), bytes_transferred);
//        std::cout << "\t\033[1;32mData message: \033[0m" << message.c_str() << "\n";
//        std::cout << "\t\033[1;32mData shm:     \033[0m" << shm_.get_data_bytes_as_string() << "\n";

        sender.send_data();

        wait_and_send_back();
    }

    void wait()
    {
        socket.async_receive(boost::asio::buffer(shm_.get_data(), shm_.get_size()),
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
    shm &shm_;
    boost::array<char, 1024> recv_buffer;
    std::string host_ip;
    udp_sender &sender;
    timer &ti;
};
