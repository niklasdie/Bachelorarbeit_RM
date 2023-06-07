//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "udp_payload.hpp"
#include "../shm/shm.cpp"
#include "../util/timer.cpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(boost::asio::io_context &io_context, shm &shm, const char *multicast_ip, int port, timer &ti)
        : io_context(io_context), shm_(shm), ti(ti)
    {
        // configuring socket
        socket.open(udp::v4());
//        socket.set_option(udp::socket::reuse_address(true));
        socket.set_option(boost::asio::ip::multicast::enable_loopback(false));
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

private:

    void handle_send(const boost::system::error_code &error,  size_t bytes_transferred)
    {
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "Send failed: " << error.message();
            return;
        }

        BOOST_LOG_TRIVIAL(debug) << "\033[1;32mSent: \033[0m" << bytes_transferred << " bytes";
    }

    /// starts the io_context
    void run_context()
    {
        io_context.run();
    }

public:

    /// sends the entire shm
    void send_data()
    {
        ti.start();
        // create payload from data of shm
        udp_payload packet(shm_, 0, sizeof(shm_struct));

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                 << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;32mPackage size: \033[0m" << 28 + packet.length;

        // send payload to multicast
        socket.async_send_to(boost::asio::buffer(
                &packet, 28 /* ip, offset and length */ + packet.length
                ),
                             multicast_endpoint,
                             boost::bind(
                                     &udp_sender::handle_send,
                                     this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred
                             )
        );
    }

    /// sends a segment of the shm
    void send_data(void *source, int length)
    {
        ti.start();
        // create payload from data of shm
        udp_payload packet(shm_, ((char *) source) - ((char *) shm_.get_data()), length);

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                 << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;32mPackage size: \033[0m" << 28 + packet.length;

        // send payload to multicast
        socket.async_send_to(boost::asio::buffer(
                                     &packet, 28 /* ip, offset and length */ + packet.length
                             ),
                             multicast_endpoint,
                             boost::bind(
                                     &udp_sender::handle_send,
                                     this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred
                             )
        );
    }

private:

    boost::asio::io_context &io_context;
    udp::socket socket{io_context};
    udp::endpoint multicast_endpoint;
    shm& shm_;
    timer &ti;
};
