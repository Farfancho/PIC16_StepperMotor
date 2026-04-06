#ifndef MOTOR_H
#define MOTOR_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    MOTOR_DIR_CW = 0,
    MOTOR_DIR_CCW = 1
} motor_dir_t;

typedef enum{
    MV_ABSOLUTE = 0,
    MV_Relative
}movement_type_t;

typedef struct
{
    int32_t position_steps; //nco
    int32_t target_steps; //0
    uint32_t step_rate_hz; //nco
    motor_dir_t direction; //0
    bool moving; //nco
    movement_type_t movement_type;
}motor_status_t;

void MotorInit(motor_status_t *motor, int32_t position_steps, int32_t target_steps, uint32_t step_rate_hz, motor_dir_t direction, bool moving, movement_type_t movement_type);

void MotorStart(void);

bool MotorIsMoving(motor_status_t *motor);

void MotorSetDirection(motor_status_t *motor, motor_dir_t direction);

bool Motor_SetStepRateHz(uint32_t step_rate_hz);
 //Configura la frecuencia del nco.
 //hz a #del nco
 //si falla retorna falso.

void MotorMoveToSteps(int32_t target_steps);

void MotorMoveRelativeSteps(int32_t delta_steps);

void MotorSetCurrentPositionSteps(int32_t position_steps);
//asigna el cero

int32_t MotorGetPositionSteps(void);
//leer posicion actual

int32_t MotorGetTargetSteps(void);

void MotorGetStatus(motor_status_t *status);

void MotorTask(void);

//perfiles de velocidad, o homing

#endif /* MOTOR_H */
