//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>

#include "udp_payload.hpp"
#include "../shm/shm.cpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(boost::asio::io_service &io_service, shm &shm, char local_ip_bytes[4], const char *multicast_ip, int port)
        : io_service(io_service), shm_(shm), socket(udp::socket(io_service))
    {
        std::memcpy(local_ip_bytes_, local_ip_bytes, 4);

        socket.open(udp::v4());
        socket.set_option(boost::asio::socket_base::send_buffer_size(1460));
        socket.set_option(udp::socket::reuse_address(true));
//        socket.set_option(boost::asio::socket_base::broadcast(true));
        broadcast_endpoint = udp::endpoint(
            address::from_string(multicast_ip),
            port
        );
//        destination_endpoint = udp::endpoint(
//                boost::asio::ip::address_v4::broadcast(),
//                port
//        );

        std::cout << "UDP sender started\n";
    }

    ~udp_sender()
    {
        socket.close();
    }

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

        udp_payload packet(local_ip_bytes_, shm_, 0, sizeof(shm_struct));

        std::cout << "\t\033[1;32mPackage data: \033[0m" << packet << "\n";
        std::cout << "\t\033[1;32mPackage size: \033[0m" << 28 + packet.length << "\n";

        socket.send_to(boost::asio::buffer(
                &packet, 28 /* offset and length */ + packet.length
        ), broadcast_endpoint);
    }

    void send_data(void *source, int length)
    {
        std::cout << "\t\033[1;42mSent:\033[0m\n";
        std::cout << "\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct() << "\n";

        udp_payload package(local_ip_bytes_, shm_, ((char *) source) - ((char *) shm_.get_data()), length);

//        if (get_buffer_size() + 12 + length > 1444) {
            std::cout << "\t\033[1;32mPackage data: \033[0m" << package << "\n";
            std::cout << "\t\033[1;32mPackage size: \033[0m" << 28 + length << "\n";

            socket.send_to(boost::asio::buffer(
                    &package, 28 /* offset and length */  + length
            ), broadcast_endpoint);
//        } else {
//            buffer.push_back(package);
//        }
    }

private:

//    size_t get_buffer_size()
//    {
//        size_t size = 0;
//        for (udp_buffer package : buffer) {
//            size += 12 + package.length;
//        }
//        return size;
//    }
//
//    std::vector<udp_buffer> buffer;
    boost::asio::io_service &io_service;
    udp::socket socket;
    udp::endpoint broadcast_endpoint;
    shm& shm_;
    char local_ip_bytes_[4];
};
