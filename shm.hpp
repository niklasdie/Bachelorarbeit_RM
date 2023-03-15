//
// Created by dev on 10.03.23.
//

#ifndef SHM_H
#define SHM_H

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

struct shm
{
    shm(int size)
    {
        shared_memory_object::remove("lsm");

        shm_o = shared_memory_object(create_only, "lsm", read_write);
        shm_o.truncate(size);

        region = mapped_region(shm_o, read_write);

        std::memset(region.get_address(), '.', region.get_size());

        std::cout << "Shared Memory created and region mapped" << std::endl
                  << "Address: " << region.get_address() << ", Length: " << region.get_size() << std::endl;
    }

//    explicit shm(const shm &shm)
//    {
//        region = mapped_region(shm.shm_o, read_write);
//    }

    ~shm()
    {
        shared_memory_object::remove("lsm");
    }

public:

    void print_data() const
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        std::cout << "Data: " << s << std::endl;
    }

    bool set_data(const std::string &data)
    {
        char *mem = static_cast<char *>(region.get_address());
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    bool set_data(const std::string &data, int offset)
    {
        char *mem = static_cast<char *>(region.get_address());
        mem += offset;
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    std::string get_data()
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data_of_shm(int offset)
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data_of_shm(int offset, int length)
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < offset + length; ++i) {
            s += *mem++;
        }
        return s;
    }

private:
    shared_memory_object shm_o;
    mapped_region region;
};

#endif