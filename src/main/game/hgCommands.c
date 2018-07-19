#include "game/hgCommands.h"

void hgSendInput() {

    // make packet
    char tPacket[16];

    // use packet type id 
    tPacket[0] = 'I';
    tPacket[1] = '7';
    tPacket[2] = 0;
    hovergamesSendPacket(tPacket, 3);
}

void hgSendGyro() {

    // get gyro data

    // make packet
    char tPacket[16];

    // use packet type id 
    tPacket[0] = 'G';
    tPacket[1] = 'T';
    tPacket[2] = 0;
    hovergamesSendPacket(tPacket, 3);
    // send packet
}


