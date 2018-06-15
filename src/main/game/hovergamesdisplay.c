
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "platform.h"

#include "io/osd.h"

#include "game/hovergamesdisplay.h"

char sTestBuffer[64];

void hovergamesUpdate(timeUs_t currentTimeUs)  {

    timeUs_t t = currentTimeUs;
    t = t;

    osdWriteString(3, 3, "HEREIAM ROFL");
    //hereiam 
}


