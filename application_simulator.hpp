//
// Created by dev on 22.05.23.
//

#ifndef SERVER_APPLICATION_SIMULATOR_HPP
#define SERVER_APPLICATION_SIMULATOR_HPP

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

struct inner_struct8 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct7 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct6 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct5 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct4 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct3 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct2 {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct inner_struct {
    const char* data1;
    const char* data2;
    const char* data3;
    const char* data4;
    const char* data5;
    const char* data6;
    int i2;
    long l2;
    double d2;
    bool b2;
    char c2;
};

struct shm_struct
{
    const char* data;
    int i;
    long l;
    double d;
    bool b;
    char c;
    inner_struct innerStruct;
    inner_struct innerStruct2;
    inner_struct innerStruct3;
    inner_struct innerStruct4;
    inner_struct innerStruct5;
    inner_struct innerStruct6;
    inner_struct innerStruct7;
    inner_struct innerStruct8;
};

static std::ostream &operator<<(std::ostream &os, const shm_struct &s)
{
    return (os << s.data << " | " <<
               s.i << " | " <<
               s.l << " | " <<
               s.d << " | " <<
               s.b << " | " <<
               s.c);
}

#endif //SERVER_APPLICATION_SIMULATOR_HPP
