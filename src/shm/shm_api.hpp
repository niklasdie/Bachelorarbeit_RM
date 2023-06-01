//
// Created by Niklas Diekhöner on 10.03.23.
//

#ifndef SHM_H
#define SHM_H

/// When the rm daemon is started this api can be used to notify the daemon of a change in the shm.
/// Then the daemon immediately synchronises the shm.

/// Notify rm daemon of a change so that the data can be synced between all nodes.
static void rm_in();

static void rm_in(const void *source, const void *destination, int length);

static void rm_out(const void *source, const void *destination, int length);

/// Directly read or write from/to the shm.
template<typename I>
static void write(const void *destination, const I *value);

template<typename O>
static O read(const void *source, const int length, const O *datatype);

#endif