#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "motor.h"
#include "eusart1.h"
#include "mcc.h"
    
typedef enum {
    CMD_NONE,
    CMD_M,//mover absoluto 
    CMD_S,//parada de emergencia
    CMD_R,//mover relativo
    CMD_F//cambiar velocidad/frecuencia
} CommandID;

void Serial_Init(motor_status_t *motor);
void Serial_Task(void);
void Serial_Timer100msISR(void);
int decodeGcode(char *input);
void SerialSendData(uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H */