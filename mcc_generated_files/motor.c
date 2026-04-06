#include <xc.h> 

#include "motor.h"

void MotorInit(motor_status_t *motor, int32_t position_steps, int32_t target_steps, uint32_t step_rate_hz, motor_dir_t direction, bool moving, movement_type_t movement_type){
    motor -> position_steps = position_steps;
    motor -> target_steps = target_steps;
    motor -> step_rate_hz = step_rate_hz;
    motor -> direction = direction;
    motor -> moving = moving;           
    motor -> movement_type = movement_type;
}

void MotorSetMoving(motor_status_t *motor, bool moving){
    NCO1CONbits.EN = moving;
    motor -> moving = moving;
}

bool MotorIsMoving(motor_status_t *motor){
    return motor -> moving;
}

void MotorSetDirection(motor_status_t *motor, motor_dir_t direction){
    LATCbits.LATC1 = direction;
    motor -> direction = direction;
}
