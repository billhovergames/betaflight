
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "platform.h"
#include "common/printf.h"
#include "io/osd.h"
#include "drivers/serial.h"
#include "drivers/serial_uart.h"
#include "drivers/system.h"
#include "drivers/time.h"

#include "io/serial.h"

#include "game/hovergamesdisplay.h"

//#define DEBUG_GAME

static serialPort_t *gameSerial;

static bool bHGameInitialized = false;

char sTestBuffer[64];
const int kBlinkRate = 60;

int iTestBufferPos = 0;
char cmdBuffer[64];

char mBlock[1024];
unsigned int mBlockPtr = 0;


typedef struct  {
    int id;
    int x;
    int y;
    char* sText;
} GameObject;

GameObject objPool[16];

int mem_page_start = 0;
int mem_page_end = 0;
int mem_cursor = 0;
unsigned int numObjects = 0;
int debug_code = 0;

int nFindObjectById(int id) {
    for(unsigned int i=0; i<numObjects; i++) {
        if(objPool[i].id == id) {
            return i;
        }
    }
    return -1;
}


void hovergamesProcessCmd(char* cmd, int len) {

    // switch first character 
    //

    strncpy(cmdBuffer, cmd, len);
    cmdBuffer[len+1] = 0;

    char command = cmdBuffer[0];

    if(command == 'C')  {
        //osdClearScreen();
        numObjects = 0;
        mBlockPtr = 0;

    } else if(command == 'D') {

        int id = cmdBuffer[1];
        debug_code = id;

        int x = cmdBuffer[2];
        int y = cmdBuffer[3];

        char* argval = &cmdBuffer[4];

        // find local copy of object
        int obj = nFindObjectById(id);

        // if not exist, add
        if(obj < 0) {

            obj = numObjects;

            objPool[obj].id = id;

            objPool[obj].x = x;
            objPool[obj].y = y;

            // alloc stext
            objPool[obj].sText = &mBlock[mBlockPtr];
            mBlockPtr += 64;
            if(mBlockPtr > 1024) {
                mBlockPtr = 0;
            }
            numObjects++;
            tfp_sprintf(objPool[obj].sText, "%s", argval );

        } else {

            // update object

            objPool[obj].x = x;
            objPool[obj].y = y;

            tfp_sprintf(objPool[obj].sText, "%s", argval );
        }

    }
}

void hovergamesDataReceive(uint16_t c, void *data) {

    UNUSED(data);

    sTestBuffer[iTestBufferPos] = c;
    iTestBufferPos++;

    // skip until packet start (char 254)
    if(iTestBufferPos == 1 && sTestBuffer[0] != 254) {
        iTestBufferPos = 0;
    }

    // check for overflow
    if(iTestBufferPos >= 64) {
        iTestBufferPos = 0;
    }

    if(iTestBufferPos > 2) {
        if(sTestBuffer[iTestBufferPos-1] == 254) {
            hovergamesProcessCmd(&sTestBuffer[1], iTestBufferPos);
            iTestBufferPos = 0;
        }
    }
}

void hovergamesInit() {
    gameSerial = uartOpen(SERIAL_PORT_IDENTIFIER_TO_UARTDEV(SERIAL_PORT_USART2), hovergamesDataReceive, NULL, HGAME_BAUDRATE, HGAME_PORT_MODE, 0); 
}


void hovergamesUpdate(timeUs_t currentTimeUs)  {
    UNUSED(currentTimeUs);

    if(!bHGameInitialized) {

        //TODO: Call somewhere else for init
        hovergamesInit();
        bHGameInitialized = true;
    }

    for(unsigned int i=0; i<numObjects; i++) {
        osdWriteString(objPool[i].x, objPool[i].y, objPool[i].sText );
    }

#ifdef DEBUG_GAME

    char statusString[64];
    char debugString[64];
    tfp_sprintf(debugString, "%d", debug_code);

    tfp_sprintf(statusString, "%d %d", numObjects, mBlockPtr);
    osdWriteString(2,3,statusString);

    osdWriteString(2,2,">>");
    osdWriteString(2,4,cmdBuffer);

    osdWriteString(2,5,debugString);

#endif

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


