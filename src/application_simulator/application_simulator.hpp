//
// Created by dev on 22.05.23.
//

#ifndef SERVER_APPLICATION_SIMULATOR_HPP
#define SERVER_APPLICATION_SIMULATOR_HPP


struct inner_struct8 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct7 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct6 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct5 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct4 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct3 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct2 {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct inner_struct {
    char data[12] = "Hello World";
    char data2;
    char data3;
    char data4;
    char data5;
    char data6;
    int i;
    long l;
    double d;
    bool b;
    char c;
};

struct shm_struct {
    char data[12] = "Hello World";
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

static std::ostream &operator<<(std::ostream &os, const shm_struct &s) {
    return (os << s.data << " | " <<
               s.i << " | " <<
               s.l << " | " <<
               s.d << " | " <<
               s.b << " | " <<
               s.c << " | " <<
               s.innerStruct.data << " | " <<
               s.innerStruct.i << " | " <<
               s.innerStruct.l << " | " <<
               s.innerStruct.d << " | " <<
               s.innerStruct.b << " | " <<
               s.innerStruct.c);
}

#endif //SERVER_APPLICATION_SIMULATOR_HPP

//#define this_is_a_long_name_for_an_int i
