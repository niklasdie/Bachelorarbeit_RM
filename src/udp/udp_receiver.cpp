//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#include "udp_payload.hpp"
#include "udp_sender.cpp"
#include "../shm/shm.cpp"
#include "../util/timer.cpp"

#define IPADDRESS "10.0.0.1" // "127.0.0.1"
#define UDP_PORT 8080

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_receiver
{
    udp_receiver(boost::asio::io_service &io_service, shm &shm, char local_ip_bytes[4], const char *broadcast_ip, int port, udp_sender &sender, timer &ti, bool resend)
            : io_service(io_service), shm_(shm), sender(sender), ti(ti)
    {
        std::memcpy(local_ip_bytes_, local_ip_bytes, 4);

        socket.open(udp::v4());
        socket.set_option(udp::socket::reuse_address(true));
        socket.bind(udp::endpoint(address::from_string(broadcast_ip), port));
//        socket.bind(udp::endpoint(boost::asio::ip::address_v4::any(), port));

        socket.set_option(boost::asio::socket_base::receive_buffer_size(1024));
        th = boost::thread(boost::bind(&udp_receiver::run_service, this));

        if(resend) {
            start_receive_and_resend();
        } else {
            start_receive();
        }

        BOOST_LOG_TRIVIAL(info) << "UDP receiver started";
    }

    ~udp_receiver()
    {
        socket.close();
        io_service.reset();
        io_service.restart();
        th.interrupt();
    }

private:

    void handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "Receive failed: " << error.message();
            return;
        }

        udp_payload packet = *(udp_payload *) &recv_buffer;

        if (std::memcmp(local_ip_bytes_, packet.src_ip, 4) != 0) {

            shm_.set_data(&packet.data, packet.offset, packet.length);

            BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;41mReceived:\033[0m"
                                     << "\n\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct()
                                     << "\n\t\033[1;31mBytes:        \033[0m" << bytes_transferred
                                     << "\n\t\033[1;31mReceived ip:  \033[0m" << ip_to_string(packet.src_ip)
                                     << "\n\t\033[1;31mPackage data: \033[0m" << packet
                                     << "\n\t\033[1;31mPackage size: \033[0m" << 28 + packet.length;

            ti.end();
        }

        start_receive();
    }

    void handle_receive_and_send_back(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "Receive failed: " << error.message();
            return;
        }

        udp_payload packet = *(udp_payload *) &recv_buffer;

        if (std::memcmp(local_ip_bytes_, packet.src_ip, 4) != 0) {

            shm_.set_data(&packet.data, packet.offset, packet.length);

            BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;41mReceived:\033[0m"
                                     << "\n\t\033[1;31mData shm:     \033[0m" << shm_.get_data_struct()
                                     << "\n\t\033[1;31mBytes:        \033[0m" << bytes_transferred
                                     << "\n\t\033[1;31mReceived ip:  \033[0m" << ip_to_string(packet.src_ip)
                                     << "\n\t\033[1;31mPackage data: \033[0m" << packet
                                     << "\n\t\033[1;31mPackage size: \033[0m" << 28 + packet.length;

//        sender.send_data((void *) &shm_.get_data_struct().data, sizeof(char[12]));
            sender.send_data();
        }

        start_receive_and_resend();
    }

    void start_receive()
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

    void start_receive_and_resend()
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

    void run_service()
    {
        io_service.run();
    }

public:

    void change_to_receive_and_resend()
    {
        socket.cancel();
        start_receive_and_resend();
    }

    void change_to_receive()
    {
        socket.cancel();
        start_receive();
    }

    void interrupt()
    {
        socket.close();
        io_service.reset();
        io_service.restart();
        th.interrupt();
    }

private:
    boost::asio::io_service &io_service;
    udp::socket socket{io_service};
    boost::thread th;
    shm &shm_;
    char recv_buffer[1460];
    udp_sender &sender;
    char local_ip_bytes_[4];
    timer &ti;
};
