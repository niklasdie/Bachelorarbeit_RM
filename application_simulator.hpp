//
// Created by dev on 22.05.23.
//

#ifndef SERVER_APPLICATION_SIMULATOR_HPP
#define SERVER_APPLICATION_SIMULATOR_HPP

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

struct inner_struct {
    std::string data1;
    std::string data2;
    std::string data3;
    std::string data4;
    std::string data5;
    std::string data6;
};

struct shm_struct
{
    std::string data;
    int i;
    long l;
    double d;
    bool b;
    char c;
    inner_struct innerStruct;
};

//std::ostream &operator<<(std::ostream &os, const shm_struct &s)
//{
//    return (os << s.data << " | " <<
//               s.i << " | " <<
//               s.l << " | " <<
//               s.d << " | " <<
//               s.b << " | " <<
//               s.c);
//}

#endif //SERVER_APPLICATION_SIMULATOR_HPP
