//
// Created by dev on 25.05.23.
//

#include "../udp/udp_receiver.cpp"

static udp_sender *udp_sender_;
static shm_o *shm_;

static void set_udp_sender(udp_sender *sender)
{
    udp_sender_ = sender;
}

static void set_shm(shm_o *shm)
{
    shm_ = shm;
}

static void stop_rm_daemon()
{
    shm_->shm_o::~shm_o();
    exit(0);
}

/// Notify rm daemon of a change so that the data can be synced between all nodes.
static void rm_in()
{
    (*udp_sender_).send_data();
}

static void rm_in(const void *source, int length)
{
    (*shm_).set_data(source, length);
    (*udp_sender_).send_data((*shm_).get_data(), length);
}

static void rm_in(const void *source, void *destination, int length)
{
    if((*shm_).set_data(source, destination, length)) {
        (*udp_sender_).send_data(destination, length);
    }
}

static void rm_in(const void *source, size_t offset, int length)
{
    (*shm_).set_data(source, offset, length);
    (*udp_sender_).send_data(offset, length);
}

template<typename T>
static void rm_in(void *destination, T &value)
{
    (*shm_).set_data(value, destination, sizeof(value));
    (*udp_sender_).send_data(destination, sizeof(value));
}

static void rm_out(const void *source, const void *destination, int length)
{
    size_t offset = (char*) source - (char*) (*shm_).get_data();
    if (offset >= 0) {
//        (*shm_).
    }

}

/// Directly read or write from/to the shm.
template<typename I>
static void write(const void *destination, const I &value)
{
    (*shm_).set_data(destination, sizeof(value));
}

template<typename I>
static void write(const void *destination, size_t length)
{
    (*shm_).set_data(destination, length);
}

template<typename O>
static O read(const void *source, const int length, const O *datatype)
{

}
