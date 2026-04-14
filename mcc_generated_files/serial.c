#include <xc.h> 

#include "serial.h"

volatile uint16_t debug_value = 0;

int decodeGcode(char *input) {
    if (strncmp(input, "S", 1) == 0) return CMD_S;
    if (strncmp(input, "R", 1) == 0) return CMD_R;
    if (strncmp(input, "F", 1) == 0) return CMD_F;
    if (strncmp(input, "M", 1) == 0) return CMD_M;
    return CMD_NONE;
}

void SerialSendData(uint16_t value){//sends Uint16 data
    while (!EUSART1_is_tx_ready());
    EUSART1_Write((uint8_t)(value & 0xFF));        // byte bajo

    while (!EUSART1_is_tx_ready());
    EUSART1_Write((uint8_t)((value >> 8) & 0xFF)); // byte alto
}

void Serial_Timer100msISR(void){
//    static uint16_t x = 0;
//    x += 100;
//    SerialSendData(x);
    SerialSendData(1000);
    STEP_Toggle();
}

void Serial_ProcessCommand(char *input)
{
    int cmd;
    cmd = decodeGcode(input);

    switch (cmd){
        case CMD_S:
            debug_value = 1000;
            break;

        case CMD_R:
            debug_value = 2000;
            break;

        case CMD_F:
            debug_value = 3000;
            break;

        case CMD_M:
            debug_value = 4000;
            break;

        case CMD_NONE:
        default:
            debug_value = 0;
            break;
    }
}


