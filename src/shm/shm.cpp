//
// Created by dev on 23.05.23.
//

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>

// header file of simulation
#include "../application_simulator/application_simulator.hpp"

#ifndef SHM
#define SHM

using namespace boost::interprocess;

struct shm
{
public:
    virtual void* get_data() = 0;
    virtual size_t get_size() = 0;
    virtual const char* get_name() = 0;
    virtual std::string get_data_bytes_as_string() = 0;
    virtual shm_struct& get_data_struct() = 0;
    virtual bool set_data(const void* src_start, size_t length) = 0;
    virtual bool set_data(const void* src_start, void* dest_start, size_t length) = 0;
    virtual bool set_data(const void* src_start, size_t offset, size_t length) = 0;
    virtual bool set_data(const shm_struct &data) = 0;

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
        // remove old shm when it exists
        shared_memory_object::remove(shm_name);

        // create new shm
        shm_obj = shared_memory_object(create_only, shm_name, read_write);
        shm_obj.truncate(sizeof(*shm_s));

        // map shm to region
        region = mapped_region(shm_obj, read_write);

        // set shm to all 0
        std::memset(region.get_address(), 0, region.get_size());

        // create new struct in shm
        shm_s = new (region.get_address()) shm_struct();

        writing = false;

        BOOST_LOG_TRIVIAL(info) << "\n\033[1;33mShm created:\n"
        << "Shared Memory created and region mapped\n"
        << "Shm Address:    " << region.get_address() << ", Shm Length:    " << region.get_size()
        << "\nObject address: " << shm_s << ", Object Length: " << sizeof(*shm_s) << "\033[0m";
    }

    ~shm_o()
    {
        /// deleting memory once is enough
//        delete shm_s;
        shared_memory_object::remove(shm_name);
    }

public:

    /// sets data in shm by start pointer and length
    bool set_data(const void* src_start, size_t length) override
    {
        while(writing | reading) {}
        writing = true;
        std::memcpy(region.get_address(), src_start, length);
        writing = false;
        return true;
    }

    /// sets data in shm by start pointer and length
    bool set_data(const void* src_start, void* dest_start, size_t length) override
    {
        while(writing | reading) {}
        int offset = (char*) dest_start - (char*) region.get_address();
        if (offset >= 0 & region.get_size() >= offset + length) {
            writing = true;
            std::memcpy(dest_start, src_start, length);
            writing = false;
            return true;
        }
        return false;
    }

    /// sets data in shm by start pointer, offset and length
    bool set_data(const void* src_start, size_t offset, size_t length) override
    {
        while(writing | reading) {}
        writing = true;
        std::memcpy((void *) (((char *) region.get_address()) + offset),  src_start, length);
        writing = false;
        return true;
    }

    /// sets data in shm by a struct object
    bool set_data(const shm_struct &data) override
    {
        while(writing | reading) {}
        writing = true;
        std::memcpy(region.get_address(), &data, region.get_size());
        writing = false;
        return true;
    }

    /// gets pointer of the start of shm
    void* get_data() override
    {
        return region.get_address();
    }

    /// gets gets the size of shm
    size_t get_size() override
    {
        return region.get_size();
    }

    /// gets name of shm
    const char* get_name() override
    {
        return shm_name;
    }

    /// gets data of shm in string representation
    std::string get_data_bytes_as_string() override
    {
        while(writing) {}
        reading = true;
        char *mem = (char *) region.get_address();
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        reading = false;
        return s;
    }

    /// gets data of shm as struct object
    shm_struct& get_data_struct() override
    {
        while(writing) {}
        return *((shm_struct*) region.get_address());
    }

private:
    shared_memory_object shm_obj;
    bool writing;
    bool reading;
    const char* shm_name;
};

#endif