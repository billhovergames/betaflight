#pragma once

#include "common/time.h"
#include "interface/hgame_protocol.h"

void hovergamesInit();
void hgSendGyro();
void hovergamesSendPacket(char* data, uint8_t len);
void hovergamesUpdate(timeUs_t currentTimeUs);
void hovergamesProcessCmd(char* cmd, int len);


