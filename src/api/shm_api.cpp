//
// Created by dev on 25.05.23.
//

#include "../udp/udp_receiver.cpp"

static udp_sender *udp_sender_;

static void set_udp_sender(udp_sender *sender)
{
    udp_sender_ = sender;
}

/// Notify rm daemon of a change so that the data can be synced between all nodes.
static void rm_in()
{
    (*udp_sender_).send_data();
}

static void rm_in(void *source, int length)
{
    (*udp_sender_).send_data(source, length);
}

static void rm_out(const void *source, const void *destination, int length)
{

}

/// Directly read or write from/to the shm.
template<typename I>
static void write(const void *destination, const I *value)
{

}

template<typename O>
static O read(const void *source, const int length, const O *datatype)
{

}
