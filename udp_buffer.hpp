//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_BUFFER_HPP
#define SERVER_UDP_BUFFER_HPP

struct udp_buffer {
    void* start_ptr;
    int length;
    char data[];

    udp_buffer(void* start, int length) : start_ptr(start), length(length)
    {

    }
};

#endif //SERVER_UDP_BUFFER_HPP
