//
// Created by Niklas Diekhöner on 13.06.23.
//

#pragma once

#include "../udp/udp_receiver.cpp"

udp_sender *udp_sender_;
shm_o *shm_;

void set_udp_sender(udp_sender *sender);
void set_shm(shm_o *shm);

