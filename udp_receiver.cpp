//
// Created by dev on 10.03.23.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "shm.hpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver
{
    udp_receiver(shm& shm, int port) : shm_(shm), port(port)
    {
    }

    ~udp_receiver() = default;

    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        std::string message = std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred);
        std::cout << "Received: '" << message << "' (" << error.message() << ")\n";

        shm_.set_data(message);

        wait();
    }

    void wait()
    {
        socket.async_receive_from(boost::asio::buffer(recv_buffer),
                                  remote_endpoint,
                                  boost::bind(
                                      &udp_receiver::handle_receive,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred
                                  )
        );
    }

    void receive() {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string("127.0.0.1"), port));

        wait();

        std::cout << "Receiving\n";
        io_service.run();
        std::cout << "Receiver exit\n";
    }

private:
    boost::asio::io_service io_service;
    udp::socket socket{io_service};
    boost::array<char, 1024> recv_buffer;
    udp::endpoint remote_endpoint;
    shm& shm_;
    int port;
};
