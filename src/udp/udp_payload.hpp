//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_PAYLOAD_HPP
#define SERVER_UDP_PAYLOAD_HPP

#include "../shm/shm.cpp"

struct udp_payload {
    unsigned int offset;       // offset of data within the shm
    unsigned int length;       // length of data
    char data[1452];           // data

    udp_payload(shm_o *shm, ptrdiff_t offset, int length) : offset(offset), length(length)
    {
        // TODO: length must be smaller than 1440-offset and bigger than 0
        // TODO: offset must be bigger than 0 and smaller than 1440
        std::memcpy((void*) &data[0], (void*) (((char *) shm->get_address()) + offset), length);
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
