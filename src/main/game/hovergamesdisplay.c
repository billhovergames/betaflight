
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "platform.h"

#include "io/osd.h"
#include "drivers/serial.h"
#include "drivers/serial_uart.h"
#include "drivers/system.h"
#include "drivers/time.h"

#include "io/serial.h"

#include "game/hovergamesdisplay.h"


static serialPort_t *gameSerial;

static bool bHGameInitialized = false;

char sTestBuffer[64];
const int kBlinkRate = 60;

int iTestBufferPos = 0;

void hovergamesDataReceive(uint16_t c, void *data) {

    UNUSED(data);
    sTestBuffer[iTestBufferPos] = c;
    iTestBufferPos++;
    if(iTestBufferPos == 64 || sTestBuffer[iTestBufferPos-1] == '\n') {
        iTestBufferPos = 0;
    }
    //hereiam 
}



void hovergamesInit() {
   gameSerial = uartOpen(SERIAL_PORT_IDENTIFIER_TO_UARTDEV(SERIAL_PORT_USART2), hovergamesDataReceive, NULL, HGAME_BAUDRATE, HGAME_PORT_MODE, 0); 

}


void hovergamesUpdate(timeUs_t currentTimeUs)  {

    if(!bHGameInitialized) {

        //TODO: Call somewhere else for init
        hovergamesInit();
        bHGameInitialized = true;
    }
    
        osdWriteString(3, 2, ">_ ");

    osdWriteString(6, 3, sTestBuffer);
    /*
    timeUs_t t = currentTimeUs;
    t = t;

    iTestCounter++;

    if(iTestCounter < kBlinkRate) {

    osdWriteString(3, 3, "HEREIAM ROFL");
    //hereiam 
    } else {
        osdClearScreen();
        if(iTestCounter >  2 * kBlinkRate) {
            iTestCounter = 0;
        }
    }*/
}


