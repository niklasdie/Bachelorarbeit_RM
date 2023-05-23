//
// Created by dev on 23.05.23.
//

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <iostream>
#include <fstream>

#include "shm.hpp"

// SM
struct shm_o: public shm
{
    explicit shm_o(int data_length, const char* shm_name) : shm_name(shm_name)
    {
        shared_memory_object::remove(shm_name);

        shm_obj = shared_memory_object(create_only, shm_name, read_write);
        shm_obj.truncate(data_length);

        region = mapped_region(shm_obj, read_write);

        std::memset(region.get_address(), 0, region.get_size());

        in_use = false;

        std::cout << "Shared Memory created and region mapped" << std::endl
                  << "Address: " << region.get_address() << ", Length: " << region.get_size() << std::endl;
    }

    ~shm_o()
    {
        shared_memory_object::remove(shm_name);
    }

public:

    template<typename I>
    void write(const void* destination, const I* value)
    {

    }

    template<typename O>
    O read(const void* source, const int length, const O* datatype)
    {

    }

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
        in_use = true;
        char *mem = static_cast<char *>(region.get_address());
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        in_use = false;
        return true;
    }

    bool set_data(const std::string &data, int offset) override
    {
        in_use = true;
        char *mem = static_cast<char *>(region.get_address());
        mem += offset;
        for (char c: data) {
            std::memset(mem, c, 1);
            *mem++;
        }
        in_use = false;
        return true;
    }

    std::string get_data() override
    {
        while(in_use) {}
        char *mem = static_cast<char *>(region.get_address());
        std::string s;
        for (std::size_t i = 0; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset) override
    {
        while(in_use) {}
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < region.get_size(); ++i) {
            s += *mem++;
        }
        return s;
    }

    std::string get_data(int offset, int length) override
    {
        while(in_use) {}
        char *mem = static_cast<char *>(region.get_address()) + offset;
        std::string s;
        for (std::size_t i = offset; i < offset + length; ++i) {
            s += *mem++;
        }
        return s;
    }

private:
    shared_memory_object shm_obj;
    bool in_use;
    const char* shm_name;
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

void rm_in(const void* source, const void* destination, int length)
{

}

void rm_out(const void* source, const void* destination, int length)
{

}
