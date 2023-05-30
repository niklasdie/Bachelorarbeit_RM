//
// Created by dev on 25.05.23.
//

#include "shm.cpp"
#include "udp_receiver.cpp"

struct shm_api
{
//    shm_api(udp_sender& u) : udp_sender_(u)
//    {
//
//    }

/// Notify rm daemon of a change so that the data can be synced between all nodes.
static void rm_in(void *source, int length)
{
    udp_sender_.send_data(source, length);
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

private:
    static udp_sender& udp_sender_;
};

