//
// Created by Niklas Diekhöner on 13.06.23.
//

#include "rm_api_private.h"
#include "rm_api.h"

/// set the udp sender for the api to use
void set_udp_sender(udp_sender *sender)
{
    udp_sender_ = sender;
}

/// set the shm for the api to use
void set_shm(shm_o *shm)
{
    shm_ = shm;
}