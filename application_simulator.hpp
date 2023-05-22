//
// Created by dev on 22.05.23.
//

#ifndef SERVER_APPLICATION_SIMULATOR_HPP
#define SERVER_APPLICATION_SIMULATOR_HPP
#include <iostream>

struct application_simulator {
    std::string data;
    int i;
    long l;
    double d;
    bool b;
    char c;

    application_simulator()
    {
        data = "Hello World";
        i = 0;
        l = 1;
        d = 0.01;
        b = false;
        c = 'a';
    };

public:

    void do_something()
    {
        data += "!";
        i++;
        ++l;
        d+= 0.01;
        b  = !b;
        if (c == 'z') {
            c = 'a';
        } else {
            c++;
        }
    }
};


#endif //SERVER_APPLICATION_SIMULATOR_HPP
