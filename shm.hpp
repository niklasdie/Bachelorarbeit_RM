//
// Created by Niklas Diekhöner on 10.03.23.
//

#ifndef SHM_H
#define SHM_H

#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

struct shm
{
public:
    virtual void print_data() const = 0;
    virtual bool set_data(const std::string &data) = 0;
    virtual bool set_data(const std::string &data, int offset) = 0;
    virtual std::string get_data() = 0;
    virtual std::string get_data(int offset) = 0;
    virtual std::string get_data(int offset, int length) = 0;

protected:
    mapped_region region;
};

/// Notify rm daemon of a change so that the data can be synced between all nodes.
void rm_in(const void* source, const void* destination, int length) = 0;
void rm_out(const void* source, const void* destination, int length) = 0;

/// Directly read or write from/to the shm.
template<typename I>
void write(const void* destination, const I* value);

template<typename O>
O read(const void* source, const int length, const O* datatype);

#endif