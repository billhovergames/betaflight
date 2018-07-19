#include "game/hgSystem.h"
#include "game/hgCommands.h"

// BUFFERS
uint8_t gamePacketBuff[HG_BUFFER_SMALL];
char sSerialBuffer[HG_BUFFER_SMALL];
char sBackBuffer[HG_BUFFER_SMALL];
char sCmdBuffer[HG_BUFFER_SMALL];
char mBlock[HG_BUFFER_LARGE];
GameObject objPool[HG_BUFFER_TINY];

// POINTERS
static serialPort_t *gameSerial;
unsigned int mBlockPtr = 0;
int mem_page_start = 0;
int mem_page_end = 0;
int mem_cursor = 0;

// COUNTERS
uint8_t gamePacketBuffLen = 0;
unsigned int iSerialBufferPos = 0;
unsigned int iTestA = 0;
unsigned int iTestB = 0;
unsigned int iTestC = 0;
unsigned int numObjects = 0;
uint16_t backBufferPtr = 0;

// STATES
volatile uint8_t serialBufferLock = HG_UNLOCKED;
int debug_code = 0;

// HELPER FUNCTIONS
int nFindObjectById(int id) {
    for(unsigned int i=0; i<numObjects; i++) {
        if(objPool[i].id == id) {
            return i;
        }
    }
    return -1;
}


void hovergamesUpdate(timeUs_t currentTimeUs)  {
    UNUSED(currentTimeUs);

    // Update displayed game objects
    for(unsigned int i=0; i<numObjects; i++) {
        osdWriteString(objPool[i].x, objPool[i].y, objPool[i].sText );
    }

    // Service Serial RX
    if(backBufferPtr > 0) {
        bool copyComplete = false;
        uint8_t maxWait = 255;

        while(!copyComplete || maxWait == 0) {
            if(serialBufferLock == HG_LOCKED) {
                // wait
                copyComplete = false;  
                maxWait--;
            } else {
                // copy to main buffer and reset ptr
                memcpy(sSerialBuffer, sBackBuffer, backBufferPtr);
                iSerialBufferPos = backBufferPtr;
                if(serialBufferLock == HG_UNLOCKED) {
                    backBufferPtr = 0;
                    copyComplete = true;  
                } else {
                    maxWait--;
                    copyComplete = false;  
                }
            }
        }
    }

    // skip until packet start (char 254)
    if(iSerialBufferPos > 0 && sSerialBuffer[0] != HG_CONTROL_CHAR) {
        iSerialBufferPos = 0;
    }
    // check for overflow
    if(iSerialBufferPos >= HG_BUFFER_SMALL) {
        iSerialBufferPos = 0;
    }

    if(iSerialBufferPos > 0) {
        hovergamesProcessCmd(&sSerialBuffer[1], iSerialBufferPos);
        iSerialBufferPos = 0;
    }

    //TODO: service serial send buffer
    if(gamePacketBuffLen > 0) {
        if(gameSerial != NULL) {
            serialWriteBuf(gameSerial, gamePacketBuff, gamePacketBuffLen);
        }
        gamePacketBuffLen = 0;
    }

    char debugString[64];
    tfp_sprintf(debugString, "TX: %d", iTestA);
    osdWriteString(2,5,debugString);
    tfp_sprintf(debugString, "OF: %d", iTestB);
    osdWriteString(2,6,debugString);
    tfp_sprintf(debugString, "RX: %d", iTestC);
    osdWriteString(2,7,debugString);

#ifdef DEBUG_GAME

    char statusString[64];

    tfp_sprintf(statusString, "%d %d", numObjects, mBlockPtr);
    osdWriteString(2,3,statusString);

    osdWriteString(2,2,">>");
    osdWriteString(2,4,sCmdBuffer);


#endif

}

void hovergamesSendPacket(char* data, uint8_t len) {

    // drop packet 
    if(len + gamePacketBuffLen + 2 > 255) {
        return;
    }

    gamePacketBuff[gamePacketBuffLen] = HG_CONTROL_CHAR;
    memcpy(&gamePacketBuff[gamePacketBuffLen+1], data, len);
    gamePacketBuff[gamePacketBuffLen + 1 + len + 1] = HG_CONTROL_CHAR;
    gamePacketBuffLen += len + 2;

    //TODO: 
    // copy to send buffer
    memcpy(gamePacketBuff, data, len);
    gamePacketBuffLen += len;
}

void hovergamesInit() {
    //gameSerial = uartOpen(SERIAL_PORT_IDENTIFIER_TO_UARTDEV(SERIAL_PORT_USART2), hovergamesDataReceive, NULL, HGAME_BAUDRATE, HGAME_PORT_MODE, 0); 
    gameSerial = NULL;
}



// ROUTINES
void hovergamesDataReceive(uint16_t c, void *data) {

    UNUSED(data);

    serialBufferLock = HG_LOCKED;

    if(backBufferPtr > HG_BUFFER_SMALL) {
        backBufferPtr = 0;
    } else {
        sBackBuffer[backBufferPtr] = c;
        backBufferPtr++;
    }
    serialBufferLock = HG_UNLOCKED;
}

void hovergamesProcessCmd(char* cmd, int len) {

    // switch first character 
    strncpy(sCmdBuffer, cmd, len);
    sCmdBuffer[len+1] = 0;

    char command = sCmdBuffer[0];

    if(command == 'C')  {
iTestA++;
        //osdClearScreen();
        numObjects = 0;
        mBlockPtr = 0;

    } else if(command == 'G') {
iTestB++;

        hgSendGyro();

    } else if(command == 'D') {
iTestC++;

        int id = sCmdBuffer[1];
        debug_code = id;

        int x = sCmdBuffer[2];
        int y = sCmdBuffer[3];

        char* argval = &sCmdBuffer[4];

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

