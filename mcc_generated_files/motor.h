#ifndef MOTOR_H
#define MOTOR_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "nco1.h"

//falta la reduccion
    
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
    int32_t position_steps; 
    volatile int32_t absolute_position;
    volatile int32_t target; 
    uint32_t step_rate_hz; 
    motor_dir_t direction; 
    volatile bool moving; 
    movement_type_t movement_type;
    int32_t steps_rev;
    int32_t reduction;
    int32_t microstepping;
}motor_status_t;

void MotorInit(motor_status_t *motor, int32_t position_steps, int32_t absolute_position, int32_t target_steps, uint32_t step_rate_hz, motor_dir_t direction, bool moving, movement_type_t movement_type);

void MotorSetMoving(motor_status_t *motor, bool moving);

bool MotorIsMoving(motor_status_t *motor);

void MotorSetDirection(motor_status_t *motor, motor_dir_t direction);

void Motor_SetStepRateHz(motor_status_t *motor, uint32_t freq);

void MotorMoveToSteps(motor_status_t *motor);

void MotorStepISR(motor_status_t *motor);

int32_t MotorGetPositionSteps(motor_status_t *motor);

int32_t MotorGetTargetSteps(motor_status_t *motor);

void MotorSetTarget(motor_status_t *motor, int32_t target);

int32_t MotorGetStepsPerOutputRevolution(motor_status_t *motor);

void MotorHoming(motor_status_t *motor);

void MotorTask(void);

//perfiles de velocidad, o homing

#endif /* MOTOR_H */
