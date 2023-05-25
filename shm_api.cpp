//
// Created by dev on 25.05.23.
//

#include "shm_api.hpp"
#include "shm.cpp"
#include "udp_receiver.cpp"

struct shm_api
{
    shm_api(shm_o& s, udp_sender& u) : shm_o_(s), udp_sender_(u)
    {

    }

private:
    shm_o& shm_o_;
    udp_sender& udp_sender_;
};

///// Notify rm daemon of a change so that the data can be synced between all nodes.
//void rm_in(const void *source, const void *destination, int length)
//{
//
//}
//
//void rm_out(const void *source, const void *destination, int length)
//{
//
//}
//
///// Directly read or write from/to the shm.
//template<typename I>
//void write(const void *destination, const I *value)
//{
//
//}
//
//template<typename O>
//O read(const void *source, const int length, const O *datatype)
//{
//
//}