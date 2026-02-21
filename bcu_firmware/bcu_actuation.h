#ifndef BCU_ACTUATION
#define BCU_ACTUATION
#include "bcu_common.h"
#ifdef ARDUINO
#include "Stepper.h"
#endif

#define STEPS_PER_REVOLUTION 2048
#define STEPPER_SPEED 12 //Keep between 10-15
#define MAX_STEPS_PER_CALL 0 //TODO: Set this!


Stepper Ballast(STEPS_PER_REVOLUTION, P_IN1, P_IN2, P_IN3, P_IN4);

int actuator_setup(){
    Ballast.setSpeed(STEPPER_SPEED);

}

int actuator_loop(){
    Ballast.step(STEPS_PER_REVOLUTION); //BLOCKING
}


#endif