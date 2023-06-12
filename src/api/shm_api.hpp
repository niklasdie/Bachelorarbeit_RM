//
// Created by Niklas Diekhöner on 10.03.23.
//

#ifndef SHM_H
#define SHM_H

/// When the rm daemon is started this api can be used to notify the daemon of a change in the shm,
/// set and get data from the rm.
/// The daemon immediately synchronises all shm when written to the rm via the api.

#include <cstddef>

/// Notify rm daemon to end work.
static void stop_rm_daemon();

/// Notify rm daemon to sync the rm data.
/// Useful for synchronising when directly written to shm.
static void sync_all_rm();
static void sync_rm(const void *src, const size_t length);
template<typename T>
static void sync_rm(const void *src);
static void sync_rm(const size_t offset, const size_t length);
template<typename T>
static void sync_rm(const size_t offset);

/// rm_in_s
/// Write to the rm and the daemon will directly sync the written data.
/// s = source, d = destination, l = length, o = offset, v = value, t = template
static void rm_in_s(const void *src);
static void rm_in_sl(const void *src, const size_t length);
template<typename T>
static void rm_in_ts(const void *src);
static void rm_in_sdl(const void *src, void *dest, const size_t length);
template<typename T>
static void rm_in_tsd(const void *src, void *dest);
static void rm_in_sol(const void *src, const size_t offset, const size_t length);
template<typename T>
static void rm_in_tso(const void *src, const size_t offset);
template<typename T>
static void rm_in_vd(void *dest, const T &value);
template<typename T>
static void rm_in_vo(size_t offset, const T &value);

/// rm_out
/// Read from the rm.
static void rm_out(void *dest);
static void rm_out(void *dest, size_t length);
template<typename T>
static void rm_out(void *dest);
static void rm_out(const void *src, void *dest, size_t length);
template<typename T>
static void rm_out(void *src, void *dest);
static void rm_out(void *dest, const size_t offset, size_t length);
template<typename T>
static void rm_out(void *dest, size_t offset);

#endif