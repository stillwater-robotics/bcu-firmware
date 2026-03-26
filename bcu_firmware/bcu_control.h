#ifndef BCU_CONTROL
#define BCU_CONTROL
#include "bcu_common.h"
//We also need sensors (collision avoidance) for this subsystem
#include "bcu_sensors.h"
//And we need the controller itself
#include "base-internal-com-api/bica_control/controller/include/common.h"
#include "base-internal-com-api/bica_control/controller/include/controller.h"

Input current_inputs;

int controller_callback_states(State current, State desired, Pose acceleration){
    current_inputs = controller(current, desired, acceleration);
    return EOK;
}

int controller_callback_inputs(Input input){
    current_inputs = input;
    return EOK;
}

int control_setup(){
    current_inputs = Input();
}

int control_loop(){
    //wow this is the hard part :)
}

#endif