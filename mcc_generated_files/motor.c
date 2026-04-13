#include <xc.h> 

#include "motor.h"

void MotorInit(motor_status_t *motor,
        int32_t position_steps, 
        int32_t absolute_position, 
        int32_t target_steps, 
        uint32_t step_rate_hz, 
        motor_dir_t direction, 
        bool moving, 
        movement_type_t movement_type,
        int32_t steps_rev,
        int32_t reduction,
        int32_t microstepping){
    motor -> position_steps = position_steps; //creo que no se usa (puede que lo elimine)
    motor -> absolute_position = absolute_position;
    motor -> target = target_steps;
    motor -> step_rate_hz = step_rate_hz;
    motor -> direction = direction;
    motor -> moving = moving;           
    motor -> movement_type = movement_type;
    motor -> steps_rev = steps_rev; //200 1.8deg
    motor -> reduction = reduction; //6
    motor -> microstepping = microstepping;//4
}

void MotorSetMoving(motor_status_t *motor, bool moving){
    NCO1CONbits.EN = moving;
    motor -> moving = moving;
}

bool MotorIsMoving(motor_status_t *motor){
    return motor -> moving;
}

void MotorSetDirection(motor_status_t *motor, motor_dir_t direction){
    LATCbits.LATC1 = (direction == MOTOR_DIR_CCW) ? 1u : 0u; //esto deja que explicitamente se pueda asignar 0 o 1 a la direccion, aunque sea un enum
    motor -> direction = direction;
}

void Motor_SetStepRateHz(motor_status_t *motor, uint32_t freq){
    SetFrequency(freq);
    motor->step_rate_hz = freq;
}
/*
 * es posible que no se necesite esta funcion dado que tengo MotorSetTarget
void MotorMoveRelativeSteps(motor_status_t *motor){
    if (motor->position_steps != motor->target_steps){
        EnStatus();
        motor->moving = true;
    }else{
        DisStatus();
        motor->moving = false;
    }
}
*/

void MotorMoveToSteps(motor_status_t *motor){
    int32_t deltaSteps;
    deltaSteps = motor->target - motor->absolute_position;
    if (deltaSteps > 0)
    {
        MotorSetDirection(motor, MOTOR_DIR_CW);
        EnStatus();
        motor->moving = true;
    }
    else if (deltaSteps < 0)
    {
        MotorSetDirection(motor, MOTOR_DIR_CCW);
        EnStatus();
        motor->moving = true;
    }
    else
    {
        DisStatus();
        motor->moving = false;
    }    
}

void MotorStepISR(motor_status_t *motor){
    if (motor == NULL) return;
    if (!motor->moving) return;

    if (motor->direction == MOTOR_DIR_CW)
    {
        motor->absolute_position++;
    }
    else
    {
        motor->absolute_position--;
    }
    
    //para el motor en caso de haber alcanzado el target
    if (motor->absolute_position == motor->target)
    {
        DisStatus();
        MotorSetMoving(motor, false);
    }
}

int32_t MotorGetPositionSteps(motor_status_t *motor){
    return motor->absolute_position;
}

int32_t MotorGetTargetSteps(motor_status_t *motor){
    return motor->target;
}

//necesito una funcion para asignar el target, si es absoluto, ponerlo directamente, y si es reativo sumarlo a la posicion absoluta.
//lo que hago es convertirlo siempre a absoluto (dado que guarda el cero y evita hacer mas funciones repetidas)

void MotorSetTarget(motor_status_t *motor, int32_t target){
    if (motor->movement_type == MV_ABSOLUTE)
    {
        motor->target = target*MotorGetStepsPerOutputRevolution(motor)/360;
    }
    else
    {
        motor->target = motor->absolute_position + target*MotorGetStepsPerOutputRevolution(motor)/360;
    }
    
}

int32_t MotorGetStepsPerOutputRevolution(motor_status_t *motor){
    return motor->steps_rev * motor->microstepping * motor->reduction;
}

void MotorHoming(motor_status_t *motor){
    MotorSetDirection(motor, MOTOR_DIR_CW);   
    MotorSetMoving(motor, true);

    while (PORTCbits.RC2 != 0) {}
    
    MotorSetMoving(motor, false);
    motor->absolute_position = 0;
}