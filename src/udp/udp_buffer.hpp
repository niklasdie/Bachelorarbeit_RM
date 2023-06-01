//
// Created by dev on 01.06.23.
//

#ifndef SERVER_UDP_BUFFER_HPP
#define SERVER_UDP_BUFFER_HPP

#include <vector>
#include "../udp/udp_payload.hpp"

struct udp_buffer {
    int count;
    int end_of_data;
    char data[1456]; // 1456

    udp_buffer()
    {
    }

    void add_payload(udp_payload &payload)
    {
        std::memcpy(&data[0] + end_of_data, &payload, 12 + payload.length);
        end_of_data += payload.length;
    }

    std::vector<udp_payload>& get_payload()
    {
        int offset = 0;
        std::vector<udp_payload> buffer;
        for (size_t i = 0; i < count; ++i) {
//            buffer.push_back(&data[0] + offset)
        }
    }
};

#endif //SERVER_UDP_BUFFER_HPP
