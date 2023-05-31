//
// Created by dev on 22.05.23.
//

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

#include "application_simulator.hpp"

using namespace boost::interprocess;

struct application_simulator
{
    application_simulator(const char *shm_name)
    {
        shm_obj = shared_memory_object(open_only, shm_name, read_write);

        region = mapped_region(shm_obj, read_write);

        shm_s = (shm_struct *) (region.get_address());

        std::cout << "\033[1;35mShm attached:\n"
            << "Shared Memory created and region mapped\n"
            << "Shm Address:    " << region.get_address() << ", Shm Length:    " << region.get_size()
            << "\nObject address: " << shm_s << ", Object Length: " << sizeof(*shm_s) << "\033[0m\n";

        (*shm_s).i = 0;
        (*shm_s).l = 100;
        (*shm_s).d = 1.1;
        (*shm_s).b = false;
        (*shm_s).c = 'a';
    };

    ~application_simulator()
    {

    }

public:

    void do_something()
    {
        (*shm_s).data[((*shm_s).i) % 11] = (*shm_s).c;
        (*shm_s).i++;
        ++(*shm_s).l;
        (*shm_s).d += 0.01;
        (*shm_s).b = !(*shm_s).b;
        if ((*shm_s).c == 'z') {
            (*shm_s).c = 'a';
        } else {
            (*shm_s).c++;
        }
    }

    shared_memory_object shm_obj;
    mapped_region region;
    shm_struct *shm_s;
};

