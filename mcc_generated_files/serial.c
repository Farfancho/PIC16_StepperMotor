#include <xc.h> 

#include "serial.h"

char rx_buffer[32];
uint8_t rx_index = 0;

uint16_t decodeGcode(char *input) {
    if (strncmp(input, "S", 1) == 0) return CMD_S;
    if (strncmp(input, "R", 1) == 0) return CMD_R;
    if (strncmp(input, "F", 1) == 0) return CMD_F;
    if (strncmp(input, "M", 1) == 0) return CMD_M;
    return CMD_NONE;
}

void SerialSendData(int16_t value){//sends int16 data
    while (!EUSART1_is_tx_ready());
    EUSART1_Write((uint8_t)(value & 0xFF));

    while (!EUSART1_is_tx_ready());
    EUSART1_Write((uint8_t)((value >> 8) & 0xFF));
}

void Serial_Timer100msISR(void){
//    static uint16_t x = 0; //debug
//    x += 100;
//    SerialSendData(x);
    SerialSendData(motor->absolute_position);
    //SerialSendData(-10);
}

void Serial_ProcessCommand(char *input)
{
    int16_t cmd;
    int32_t value = 0;

    cmd = decodeGcode(input);

    switch (cmd)
    {
        case CMD_S:
            MotorEmergencyStop(motor);
            break;

        case CMD_R:
            // movimiento relativo en grados, por ejemplo: R90 o R-45
            value = atoi(&input[1]);
            motor->movement_type = MV_RELATIVE;
            MotorSetTarget((motor_status_t *)motor, value);
            MotorMoveToSteps((motor_status_t *)motor);
            break;

        case CMD_F:
            // cambio de frecuencia, por ejemplo: F1200
            value = atoi(&input[1]);
            Motor_SetStepRateHz((motor_status_t *)motor, (uint32_t)value);
            break;

        case CMD_M:
            // movimiento absoluto en grados, por ejemplo: M180
            value = atoi(&input[1]);
            motor->movement_type = MV_ABSOLUTE;
            MotorSetTarget((motor_status_t *)motor, value);
            MotorMoveToSteps((motor_status_t *)motor);
            break;

        case CMD_NONE:
        default:
            break;
    }
}

void Serial_Task(void)
{
    char c;

    if (PIR3bits.RC1IF)
    {
        c = EUSART1_Read();

        if (c == '\n')
        {
            rx_buffer[rx_index] = '\0';
            Serial_ProcessCommand(rx_buffer);
            rx_index = 0;
        }
        else if (rx_index < sizeof(rx_buffer) - 1)
        {
            rx_buffer[rx_index++] = c;
        }
    }
}