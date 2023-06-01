//
// Created by dev on 30.05.23.
//

#ifndef SERVER_UDP_PAYLOAD_HPP
#define SERVER_UDP_PAYLOAD_HPP

#include "../shm/shm.cpp"

struct udp_payload {
    ptrdiff_t offset;
    int length;
    char data[1444]; // 1444

    udp_payload(shm &shm, ptrdiff_t offset, int length) : offset(offset), length(length)
    {
        // TODO: length must be smaller than 1444-offset and bigger than 0
        // TODO: offset must be bigger than 0 and smaller than 1443
        std::memcpy((void*) &data[0],  (void*) (((char *) shm.get_data()) + offset), length);
    }

};

static std::ostream &operator<<(std::ostream &os, const unsigned char data[]) {
    for (std::size_t i = 0; i < 1444; ++i) {
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
