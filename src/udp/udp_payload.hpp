//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_PAYLOAD_HPP
#define SERVER_UDP_PAYLOAD_HPP

#include "../shm/shm.cpp"

struct udp_payload {
    unsigned int offset;       // offset of data within the shm
    unsigned int length;       // length of data
    char data[1464];           // data

    udp_payload(shm_o &shm, const ptrdiff_t offset, const int length) : offset(offset), length(length)
    {
        // TODO: offset must be bigger than 0 and smaller than 1464
        // TODO: length must be smaller than 1464-offset and bigger than 0
        std::memcpy((void*) &data[0], (void*) (((char *) shm.get_address()) + offset), length);
    }

};

static std::ostream &operator<<(std::ostream &os, const unsigned char data[]) {
    for (std::size_t i = 0; i < 1464; ++i) {
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
