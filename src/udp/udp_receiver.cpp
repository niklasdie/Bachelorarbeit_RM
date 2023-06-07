//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/socket_base.hpp>

#include "udp_payload.hpp"
#include "udp_sender.cpp"
#include "../shm/shm.cpp"

#define IPADDRESS "10.0.0.1" // "127.0.0.1"
#define UDP_PORT 8080

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver
{
    udp_receiver(boost::asio::io_context &io_context, shm &shm, const char *multicast_ip, int port, udp_sender &sender, timer &ti, bool resend)
            : io_context(io_context), shm_(shm), sender(sender), ti(ti)
    {
        // configuring socket
        socket.open(udp::v4());
//        socket.set_option(udp::socket::reuse_address(true));
//        socket.set_option(boost::asio::ip::multicast::enable_loopback(false));
        socket.set_option(boost::asio::socket_base::receive_buffer_size(14600));
        socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::make_address(multicast_ip)));
        BOOST_LOG_TRIVIAL(debug) << "Multicast address: " << multicast_ip;
        socket.bind(udp::endpoint(address::from_string(multicast_ip), port));

        // start io_context in new thread
        th_io_context_run = boost::thread(boost::bind(&udp_receiver::run_context, this));

        // start receive
        if(resend) {
            start_async_receive_and_resend();
        } else {
            start_async_receive();
        }

        BOOST_LOG_TRIVIAL(info) << "UDP receiver started";
    }

    ~udp_receiver()
    {
        socket.close();
        io_context.reset();
        io_context.restart();
        th_io_context_run.join();
        th_handle_receive.join();
    }

private:

    /// handles the receive data by writing it into the shm
    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "Receive failed: " << error.message();
            return;
        }

        udp_payload packet = *(udp_payload *) &recv_buffer;

        shm_.set_data(&packet.data, packet.offset, packet.length);

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;41mReceived:\033[0m"
                                 << "\n\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;31mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;31mPackage size: \033[0m" << 28 + packet.length
                                 << "\n\t\033[1;31mBytes:        \033[0m" << bytes_transferred;

        ti.end();

        start_async_receive();
    }

    /// handles the receive data by writing it into the shm and resending it to the source
    void handle_receive_and_send_back(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "Receive failed: " << error.message();
            return;
        }

        udp_payload packet = *(udp_payload *) &recv_buffer;
        shm_.set_data(&packet.data, packet.offset, packet.length);

        BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;41mReceived:\033[0m"
                                 << "\n\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct()
                                 << "\n\t\033[1;31mPackage data: \033[0m" << packet
                                 << "\n\t\033[1;31mPackage size: \033[0m" << 28 + packet.length
                                 << "\n\t\033[1;31mBytes:        \033[0m" << bytes_transferred;

        ti.end();

        sender.send_data();

        start_async_receive_and_resend();
    }

    /// starts the async receive
    void start_async_receive()
    {
        socket.async_receive(boost::asio::buffer(recv_buffer),
                             boost::bind(
                                     &udp_receiver::handle_receive,
                                     this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred
                             )
        );
    }

    /// starts the async receive and resend to source
    void start_async_receive_and_resend()
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

    /// starts the io_context
    void run_context()
    {
        io_context.run();
    }

public:

    /// changes to receive and resend to source
    void change_to_receive_and_resend()
    {
        socket.cancel();
        start_async_receive_and_resend();
    }

    /// changes to only receive
    void change_to_receive()
    {
        socket.cancel();
        start_async_receive();
    }

    /// interrupts all actions
    void interrupt()
    {
        socket.close();
        io_context.reset();
        io_context.restart();
        th_io_context_run.interrupt();
        th_handle_receive.interrupt();
    }

private:
    boost::asio::io_context &io_context;
    udp::socket socket{io_context};
    boost::thread th_io_context_run;
    boost::thread th_handle_receive;
    shm &shm_;
    char recv_buffer[1460];
    udp_sender &sender;
    timer &ti;
};
