//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>

#include "udp_buffer.hpp"
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

    // TODO
    /// Send segments of shm.

    // TODO
    /// Fill a buffer with data to send and send it at once cyclic
    /// or send it if buffer is full.

    // TODO
    /// Write own buffer with {count, [package1], [package2], ...}.
    /// [package] = {start_ptr, length, data}

    void send_data()
    {
        std::cout << "\t\033[1;42mSent:\033[0m\n";
        std::cout << "\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct() << "\n";

        udp_buffer package(shm_, 0, shm_.get_size());

//        std::cout << "\t\033[1;32msizeof(package): \033[0m" << sizeof(package) << "\n";
//        std::cout << "\t\033[1;32mPackage data: \033[0m" << package << "\n";

        socket.send_to(boost::asio::buffer(
                &package, sizeof(package)
        ), destination_endpoint);

//        socket.send_to(boost::asio::buffer(
//            shm_.get_data(), shm_.get_size()
//        ), destination_endpoint);
    }

    void send_data(void *source, int length)
    {
        std::cout << "\t\033[1;42mSent:\033[0m\n";
        std::cout << "\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct() << "\n";

        udp_buffer package(shm_, ((char *) shm_.get_data()) - ((char *) source), length);

        socket.send_to(boost::asio::buffer(
                &package, sizeof(package)
        ), destination_endpoint);

    }

private:
    boost::asio::io_context io_context;
    udp::socket socket;
    udp::endpoint destination_endpoint;
    shm& shm_;
};
