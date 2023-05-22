//
// Created by Niklas Diekhöner on 10.03.23.
//

#ifndef SHM_H
#define SHM_H

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <iostream>
#include <fstream>

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

// SM
template<typename T>
struct shm_o: public shm
{
    explicit shm_o(T data)
    {
        shared_memory_object::remove("lsm");

        shm_obj = shared_memory_object(create_only, "lsm", read_write);
        shm_obj.truncate(data);
//        shm_obj.

        region = mapped_region(shm_obj, read_write);

        std::memset(region.get_address(), '.', region.get_size());

        std::cout << "Shared Memory created and region mapped" << std::endl
                  << "Address: " << region.get_address() << ", Length: " << region.get_size() << std::endl;
    }

    ~shm_o()
    {
        shared_memory_object::remove("lsm");
    }

public:

    void print_data() const override
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        std::cout << "Data: " << s << std::endl;
    }

    bool set_data(const std::string &data) override
    {
        char *mem = static_cast<char *>(region.get_address());
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    bool set_data(const std::string &data, int offset) override
    {
        char *mem = static_cast<char *>(region.get_address());
        mem += offset;
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    std::string get_data() override
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset) override
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset, int length) override
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < offset + length; ++i) {
            s += *mem++;
        }
        return s;
    }

private:
    shared_memory_object shm_obj;
};

// SM for a file
struct shm_f: public shm
{
    explicit shm_f(const std::string &file, int size)
    {
        {  //Create a file
//            file_mapping::remove(file.c_str());
            std::filebuf fbuf;
            fbuf.open(file, std::ios_base::in | std::ios_base::out
                                | std::ios_base::trunc | std::ios_base::binary);
            //Set the size
            fbuf.pubseekoff(size, std::ios_base::beg);
            fbuf.sputc(0);
        }

        shm_file = file_mapping(file.c_str(), read_write);

        region = mapped_region(shm_file, read_write, 0, size);

//        std::memset(region.get_address(), '.', region.get_size());

        std::cout << "File mapping created and region mapped" << std::endl
                  << "Address: " << region.get_address() << ", Length: " << region.get_size() << std::endl;
    }

    ~shm_f() = default;

public:

    void print_data() const override
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        std::cout << "Data: " << s << std::endl;
    }

    bool set_data(const std::string &data) override
    {
        char *mem = static_cast<char *>(region.get_address());
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    bool set_data(const std::string &data, int offset) override
    {
        char *mem = static_cast<char *>(region.get_address());
        mem += offset;
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        return true;
    }

    std::string get_data() override
    {
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset) override
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset, int length) override
    {
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < offset + length; ++i) {
            s += *mem++;
        }
        return s;
    }

private:
    file_mapping shm_file;
};

#endif