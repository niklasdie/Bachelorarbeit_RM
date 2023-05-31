//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_BUFFER_HPP
#define SERVER_UDP_BUFFER_HPP

#include "shm.cpp"

struct udp_buffer {
    ptrdiff_t offset;
    int length;
    unsigned char data[1444]; // 1444

    udp_buffer(shm &shm, ptrdiff_t offset, int length) : offset(offset), length(length)
    {
        std::memcpy(&data[0], ((char *) shm.get_data()) + offset, length);
    }

};

static std::ostream &operator<<(std::ostream &os, const unsigned char data[]) {
    for (std::size_t i = 0; i < 1444; ++i) {
        os << data[i];
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const udp_buffer &b) {
    return (os << b.offset << " | " <<
            b.length << " | " <<
            b.data);
}

#endif //SERVER_UDP_BUFFER_HPP
