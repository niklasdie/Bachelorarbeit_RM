//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_PAYLOAD_HPP
#define SERVER_UDP_PAYLOAD_HPP

#include "../shm/shm.cpp"

struct udp_payload {
    char src_ip[4];         // source ip address of packet
    ptrdiff_t offset;       // offset of data within the shm
    int length;             // length of data
    char data[1440];        // data

    udp_payload(char ip[4], shm &shm, ptrdiff_t offset, int length) : offset(offset), length(length)
    {
        // TODO: length must be smaller than 1440-offset and bigger than 0
        // TODO: offset must be bigger than 0 and smaller than 1440
        std::memcpy((void*) &src_ip[0],  (void*) &ip[0], 4);
        std::memcpy((void*) &data[0],  (void*) (((char *) shm.get_data()) + offset), length);
    }

};

static std::ostream &operator<<(std::ostream &os, const unsigned char data[]) {
    for (std::size_t i = 0; i < 1440; ++i) {
        os << data[i];
    }
    return os;
}

static std::ostream &operator<<(std::ostream &os, const udp_payload &b) {
    return (os << b.offset << " | " <<
            b.length << " | " <<
            b.data);
}

#endif //SERVER_UDP_PAYLOAD_HPP
