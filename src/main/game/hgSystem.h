#pragma once
//hgSystem.h

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "platform.h"
#include "common/printf.h"
#include "io/osd.h"
#include "drivers/serial.h"
#include "drivers/serial_uart.h"
#include "drivers/system.h"
#include "drivers/time.h"

#include "io/serial.h"

#include "interface/hgame_protocol.h"

// SETTINGS
//#define DEBUG_GAME
#define HG_BUFFER_LARGE 1024
#define HG_BUFFER_SMALL 255
#define HG_BUFFER_TINY 16

// CONSTANTS
#define HG_CONTROL_CHAR 254
#define HG_MAIN_BUFFER_SIZE 1024

// ENUMS
enum {
    HG_HEARTBEAT,
    HG_LOG,
    HG_REPORT_GYRO,
    HG_REPORT_RC,
    HG_REPORT_RSSI
} HGPacketTypes;

enum {
    HG_LOCK_SERVICE,
    HG_LOCK_COMMUNICATION,
    HG_UNLOCKED,
    HG_LOCKED
};

// TYPES
typedef struct  {
    uint8_t id;
    uint8_t x;
    uint8_t y;
    char* sText;
} GameObject;

void hovergamesDataReceive(uint16_t c, void *data);
void hovergamesUpdate(timeUs_t currentTimeUs);
void hovergamesSendPacket(char* data, uint8_t len);
void hovergamesInit();
void hovergamesProcessCmd(char* cmd, int len);
