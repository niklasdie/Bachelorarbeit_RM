//
// Created by dev on 23.05.23.
//

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

// header file of simulation
#include "application_simulator.hpp"

#ifndef SHM
#define SHM

struct shm
{
public:
    virtual bool set_data(const char* data) = 0;
    virtual bool set_data(const shm_struct &data) = 0;
    virtual std::string get_data_bytes_as_string() = 0;
    virtual shm_struct get_data_struct() = 0;

protected:
    mapped_region region;
    // struct that is mapped to shm
    shm_struct *shm_s;
};

// SM
struct shm_o: shm
{
    explicit shm_o(const char* shm_name) : shm_name(shm_name)
    {
        shared_memory_object::remove(shm_name);

        shm_obj = shared_memory_object(create_only, shm_name, read_write);
        shm_obj.truncate(sizeof(*shm_s));

        region = mapped_region(shm_obj, read_write);

        std::memset(region.get_address(), 0, region.get_size());

        shm_s = new (region.get_address()) shm_struct();

        in_use = false;

        std::cout << "\033[1;33mShm created:\n"
        << "Shared Memory created and region mapped\n"
        << "Shm Address:    " << region.get_address() << ", Shm Length:    " << region.get_size()
        << "\nObject address: " << shm_s << ", Object Length: " << sizeof(*shm_s) << "\033[0m\n";
    }

    ~shm_o()
    {
        // deleting memory once is enough
//        delete shm_s;
        shared_memory_object::remove(shm_name);
    }

public:

    bool set_data(const char* data) override
    {
        in_use = true;
        char *mem = static_cast<char *>(region.get_address());
        for (size_t i = 0; data[i] != '\0'; ++i) {
            std::memset(mem, data[i], 1);
            *mem++;
        }
        in_use = false;
        return true;
    }

    bool set_data(const shm_struct &data) override
    {
        in_use = true;
        std::memcpy(region.get_address(), &data, region.get_size());
        in_use = false;
        return true;
    }

    std::string get_data_bytes_as_string() override
    {
        while(in_use) {}
        char *mem = (char *) region.get_address();
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    shm_struct get_data_struct() override
    {
        while(in_use) {}
        return *((shm_struct*) region.get_address());
    }

private:
    shared_memory_object shm_obj;
    bool in_use;
    const char* shm_name;
};

#endif