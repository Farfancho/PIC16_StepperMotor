#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "motor.h"

void Serial_Init(motor_status_t *motor);
void Serial_Task(void);
void Serial_Timer100msISR(void);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H */