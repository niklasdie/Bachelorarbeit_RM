//
// Created by dev on 25.05.23.
//

#include "rm_api.h"
#include "rm_api_private.h"

// public
/// Notify rm daemon to end work.
void stop_rm_daemon()
{
    shm_->shm_o::~shm_o();
    exit(0);
}

/// Notify rm daemon to sync the rm data.
void sync_all_rm()
{
    (*udp_sender_).send_data();
}

void sync_rm(const void *src, const size_t length)
{
    (*udp_sender_).send_data(src, length);
}

template<typename T>
void sync_rm(const void *src)
{
    (*udp_sender_).send_data(src, sizeof(T));
}

void sync_rm(const size_t offset, const size_t length)
{
    (*udp_sender_).send_data(offset, length);
}

template<typename T>
void sync_rm(const size_t offset)
{
    (*udp_sender_).send_data(offset, sizeof(T));
}


/// rm_in_s
/// Write to the rm and the daemon will directly sync the written data.

void rm_in_s(const void *src)
{
    if((*shm_).set_data(src)) {
        (*udp_sender_).send_data();
    }
}

void rm_in_sl(const void *src, const size_t length)
{
    if((*shm_).set_data(src, length)) {
        (*udp_sender_).send_data((*shm_).get_data(), length);
    }
}

template<typename T>
void rm_in_ts(const void *src)
{
    if((*shm_).set_data(src, sizeof(T))) {
        (*udp_sender_).send_data((*shm_).get_data(), sizeof(T));
    }
}

void rm_in_sdl(const void *src, void *dest, const size_t length)
{
    if((*shm_).set_data(src, dest, length)) {
        (*udp_sender_).send_data(dest, length);
    }
}

template<typename T>
void rm_in_tsd(const void *src, void *dest)
{
    if((*shm_).set_data(src, dest, sizeof(T))) {
        (*udp_sender_).send_data(dest, sizeof(T));
    }
}

void rm_in_sol(const void *src, const size_t offset, const size_t length)
{
    if((*shm_).set_data(src, offset, length)) {
        (*udp_sender_).send_data(offset, length);
    }
}

template<typename T>
void rm_in_tso(const void *src, const size_t offset)
{
    if((*shm_).set_data(src, offset, sizeof(T))) {
        (*udp_sender_).send_data(offset, sizeof(T));
    }
}

template<typename T>
void rm_in_vd(void *dest, const T &value)
{
    if((*shm_).set_data(value, dest, sizeof(value))) {
        (*udp_sender_).send_data(dest, sizeof(value));
    }
}

template<typename T>
void rm_in_vo(size_t offset, const T &value)
{
    if((*shm_).set_data(value, offset, sizeof(value))) {
        (*udp_sender_).send_data(offset, sizeof(value));
    }
}

/// rm_out
/// Read from the rm.

void rm_out(void *dest)
{
    (*shm_).get_data(dest);
}

void rm_out(void *dest, size_t length)
{
    (*shm_).get_data(dest, length);
}

template<typename T>
void rm_out(void *dest)
{
    (*shm_).get_data(dest,sizeof(T));
}

void rm_out(const void *src, void *dest, size_t length)
{
    (*shm_).get_data(src, dest, length);
}

template<typename T>
void rm_out(void *src, void *dest)
{
    (*shm_).get_data(src, dest, sizeof(T));
}

void rm_out(void *dest, const size_t offset, size_t length)
{
    (*shm_).get_data(dest, offset, length);
}

template<typename T>
void rm_out(void *dest, size_t offset)
{
    (*shm_).get_data(dest, offset, sizeof(T));
}
