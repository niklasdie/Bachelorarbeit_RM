//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>

#include "udp_payload.hpp"
#include "../shm/shm.cpp"
#include "../util/timer.cpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

static std::string ip_to_string(char ip[4]);
static std::string ip_to_string(const char ip[4]);

struct udp_sender
{
    udp_sender(boost::asio::io_context &io_context, shm &shm, char local_ip_bytes[4], const char *multicast_ip, int port, timer &ti)
        : io_context(io_context), shm_(shm), ti(ti)
    {
        std::memcpy(local_ip_bytes_, local_ip_bytes, 4);

        // configuring socket
        socket.open(udp::v4());
        socket.set_option(udp::socket::reuse_address(true));
        socket.set_option(boost::asio::socket_base::send_buffer_size(14600));
        socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::make_address(multicast_ip)));

        // configuring endpoint
        multicast_endpoint = udp::endpoint(
            address::from_string(multicast_ip),
            port
        );

        BOOST_LOG_TRIVIAL(info) << "UDP sender started";
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

    /// sends the entire shm
    void send_data()
    {
        ti.start();
        // create payload from data of shm
        udp_payload packet(local_ip_bytes_, shm_, 0, sizeof(shm_struct));

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                 << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;32mSend ip:      \033[0m" << ip_to_string(packet.src_ip)
                                 << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;32mPackage size: \033[0m" << 28 + packet.length;

        BOOST_LOG_TRIVIAL(debug) << "Sending: " << ip_to_string(packet.src_ip);
        // send payload to multicast
        socket.send_to(boost::asio::buffer(
                &packet, 28 /* ip, offset and length */ + packet.length
        ), multicast_endpoint);
    }

    /// sends a segment of the shm
    void send_data(void *source, int length)
    {
        ti.start();
        // create payload from data of shm
        udp_payload packet(local_ip_bytes_, shm_, ((char *) source) - ((char *) shm_.get_data()), length);

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                 << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;32mSend ip:      \033[0m" << ip_to_string(packet.src_ip)
                                 << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;32mPackage size: \033[0m" << 28 + packet.length;

        // send payload to multicast
        socket.send_to(boost::asio::buffer(
                &packet, 28 /* ip, offset and length */  + length
        ), multicast_endpoint);
    }

private:

    boost::asio::io_context &io_context;
    udp::socket socket{io_context};
    udp::endpoint multicast_endpoint;
    shm& shm_;
    char local_ip_bytes_[4];
    timer &ti;
};

static std::string ip_to_string(char ip[4])
{
    std::stringstream ss;
    ss << (unsigned int) ip[0] << "." << (unsigned int) ip[1] << "." << (unsigned int) ip[2] << "." << (unsigned int) ip[3];
    return ss.str();
}

static std::string ip_to_string(const char ip[4])
{
    std::stringstream ss;
    ss << (unsigned int) ip[0] << "." << (unsigned int) ip[1] << "." << (unsigned int) ip[2] << "." << (unsigned int) ip[3];
    return ss.str();
}
