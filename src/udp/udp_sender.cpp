//
// Created by Niklas Diekhöner on 10.03.23.
//

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "udp_payload.hpp"
#include "../shm/shm.cpp"
#include "../util/timer.cpp"

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct udp_sender
{
    udp_sender(boost::asio::io_context &io_context, shm_o &shm, const char *multicast_ip, int port, timer &ti)
        : io_context(io_context), shm_(shm), ti(ti)
    {
        // configuring socket
        socket.open(udp::v4());
        socket.set_option(boost::asio::ip::multicast::enable_loopback(false));
        socket.set_option(boost::asio::socket_base::send_buffer_size(146000));
        socket.set_option(boost::asio::socket_base::linger(false, 0));
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

//private:
//
//    void handle_send(const boost::system::error_code &error,  size_t bytes_transferred)
//    {
//        if (error) {
//            BOOST_LOG_TRIVIAL(error) << "Send failed: " << error.message();
//            return;
//        }
//
//        BOOST_LOG_TRIVIAL(debug) << "\033[1;32mSent: \033[0m" << bytes_transferred << " bytes";
//    }

public:

    /// sends the entire shm
    void send_data()
    {
        if(sizeof(shm_struct) <= 1452) { // data fits into one diagram

            // create payload from data of shm
            udp_payload packet(shm_, 0, sizeof(shm_struct));

            BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                     << "\n\t\033[1;32mData shm:         \033[0m" << shm_.get_data_struct()
                                     << "\n\t\033[1;32mPackage data:     \033[0m" << packet
                                     << "\n\t\033[1;32mPackage size:     \033[0m" << sizeof(packet);

            ti.start();

            // send payload to multicast
            socket.send_to(boost::asio::buffer(
                                   &packet, 8 + packet.length
                           ),
                           multicast_endpoint
            );

        } else { // data not fits into one diagram
            int offset = 0;

            while (offset < sizeof(shm_struct)) {
                int length = (sizeof(shm_struct) - offset >= 1452) ? 1452 : sizeof(shm_struct) - offset;

                // create payload from data of shm
                udp_payload packet(shm_, offset, length);

                BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                         << "\n\t\033[1;32mData shm:         \033[0m" << shm_.get_data_struct()
                                         << "\n\t\033[1;32mPackage data:     \033[0m" << packet
                                         << "\n\t\033[1;32mPackage size:     \033[0m" << sizeof(packet);

                ti.start();

                // send payload to multicast
                socket.send_to(boost::asio::buffer(
                                       &packet, 8 + length
                               ),
                               multicast_endpoint
                );
                offset += length;
            }
        }
    }

    /// sends a segment of the shm
    void send_data(const void *source, size_t length)
    {
        if(sizeof(shm_struct) <= 1452) { // data fits into one diagram

            // create payload from data of shm
            udp_payload packet(shm_, ((char *) source) - ((char *) shm_.get_address()), length);

            BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                     << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                     << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                     << "\n\t\033[1;32mPackage size: \033[0m" << sizeof(packet);

            ti.start();

            // send payload to multicast
            socket.send_to(boost::asio::buffer(
                                   &packet, 8 + packet.length
                           ),
                           multicast_endpoint
            );
        } else { // data not fits into one diagram
            int offset = 0;

            while (offset < length) {
                int length_ = (length - offset >= 1452) ? 1452 : length - offset;

                // create payload from data of shm
                udp_payload packet(shm_, ((char *) source) - ((char *) shm_.get_address()) + offset, length_);

                BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                         << "\n\t\033[1;32mData shm:         \033[0m" << shm_.get_data_struct()
                                         << "\n\t\033[1;32mPackage data:     \033[0m" << packet
                                         << "\n\t\033[1;32mPackage size:     \033[0m" << sizeof(packet);

                ti.start();

                // send payload to multicast
                socket.send_to(boost::asio::buffer(
                                       &packet, 8 + length_
                               ),
                               multicast_endpoint
                );
                offset += length_;
            }
        }
    }

    /// sends a segment of the shm
    void send_data(const size_t offset, const size_t length)
    {

        if(sizeof(shm_struct) <= 1452) { // data fits into one diagram

            // create payload from data of shm
            udp_payload packet(shm_, offset, length);

            BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                     << "\n\t\033[1;32mData shm:     \033[0m" << shm_.get_data_struct()
                                     << "\n\t\033[1;32mPackage data: \033[0m" << packet
                                     << "\n\t\033[1;32mPackage size: \033[0m" << sizeof(packet);

            ti.start();

            // send payload to multicast
            socket.send_to(boost::asio::buffer(
                                   &packet, 8 + packet.length
                           ),
                           multicast_endpoint
            );
        } else { // data not fits into one diagram
            int offset_ = 0;

            while (offset_ < length) {
                int length_ = (length - offset_ >= 1452) ? 1452 : length - offset_;

                // create payload from data of shm
                udp_payload packet(shm_, offset + offset_, length_);

                BOOST_LOG_TRIVIAL(debug) << "\n\t\033[1;42mSent:\033[0m"
                                         << "\n\t\033[1;32mData shm:         \033[0m" << shm_.get_data_struct()
                                         << "\n\t\033[1;32mPackage data:     \033[0m" << packet
                                         << "\n\t\033[1;32mPackage size:     \033[0m" << sizeof(packet);

                ti.start();

                // send payload to multicast
                socket.send_to(boost::asio::buffer(
                                       &packet, 8 + length_
                               ),
                               multicast_endpoint
                );
                offset_ += length_;
            }
        }
    }

private:

    boost::asio::io_context &io_context;
    udp::socket socket{io_context};
    udp::endpoint multicast_endpoint;
    shm_o &shm_;
    timer &ti;
};
