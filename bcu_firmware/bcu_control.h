#ifndef BCU_CONTROL
#define BCU_CONTROL
#include "bcu_common.h"
//We also need sensors (collision avoidance) for this subsystem
#include "bcu_sensors.h"
//And we need the controller itself
#include "base-internal-com-api/bica_control/controller/include/common.h"
#include "base-internal-com-api/bica_control/controller/include/controller.h"
// Aaaand we also need the stepper and servo libraries to drive our escs/ballasts
#ifdef ARDUINO
#include "Stepper.h"
#include "Servo.h"
#endif

// Utility Definitions
#define STEPS_PER_REVOLUTION 2048
#define STEPPER_SPEED 12 //Keep between 10-15
#define MAX_STEPS_PER_CALL 15 //TODO: Set this!
#define CM_PER_REV 1.0
#define ESC_BWD_MAX 1000.0
#define ESC_NEUTRAL 1500.0
#define ESC_FWD_MAX 2000.0
//TODO: Correct these three numbers/constants
#define ESC_MAX_RPS 5000.0
#define ESC_MIN_RPS -5000.0
// 1 or -1
#define POSITIVE_STEP_DIR (-1)

// Actuators
Stepper ballast(STEPS_PER_REVOLUTION, P_IN1, P_IN2, P_IN3, P_IN4);
Servo esc_right, esc_left;

// Control/Objective Storage
Input current_inputs;
int esc_left_drive, esc_right_drive;
int ballast_pos_current, ballast_pos_desired;

void update_drive(){
    if(current_inputs.left > 0.0)
        esc_left_drive = ESC_NEUTRAL + current_inputs.left * (ESC_FWD_MAX - ESC_NEUTRAL)/ESC_MAX_RPS;
    else
        esc_left_drive = ESC_NEUTRAL + current_inputs.left * (ESC_NEUTRAL - ESC_BWD_MAX)/ESC_MIN_RPS;

    if(current_inputs.right > 0.0)
        esc_right_drive = ESC_NEUTRAL + current_inputs.right * (ESC_FWD_MAX - ESC_NEUTRAL)/ESC_MAX_RPS;
    else
        esc_right_drive = ESC_NEUTRAL + current_inputs.right * (ESC_NEUTRAL - ESC_BWD_MAX)/ESC_MIN_RPS;
    
    ballast_pos_desired = round(STEPS_PER_REVOLUTION * (current_inputs.ballast/CM_PER_REV));

    esc_left_drive = min(ESC_FWD_MAX, max(ESC_BWD_MAX, esc_left_drive));
    esc_right_drive = min(ESC_FWD_MAX, max(ESC_BWD_MAX, esc_right_drive));
    ballast_pos_desired = min(ballast_pos_desired, 0.0);
}

int bcu_controller_callback_states(State current, State desired, Pose acceleration){
    current_inputs = controller(current, desired, acceleration);
    update_drive();
    return EOK;
}

int bcu_controller_callback_inputs(Input input){
    current_inputs = input;
    update_drive();
    return EOK;
}

int control_setup(){
    esc_left.attach(P_ESC_L);
    esc_right.attach(P_ESC_R);
    current_inputs = Input(0, 0, 0);
    update_drive();

    // Initialize ESCs
    esc_left.writeMicroseconds(ESC_NEUTRAL);
    esc_right.writeMicroseconds(ESC_NEUTRAL);
    delay(2000);
}

int control_loop(){
    // ESC Control
    esc_left.writeMicroseconds(esc_left_drive);
    esc_right.writeMicroseconds(esc_right_drive);

    // Ballast Control
    if(ballast_pos_current < ballast_pos_desired){
        int steps_to_take = min(ballast_pos_desired - ballast_pos_current, MAX_STEPS_PER_CALL);
        ballast.step(POSITIVE_STEP_DIR*steps_to_take);
    }else{
        int steps_to_take = max(ballast_pos_desired - ballast_pos_current, -MAX_STEPS_PER_CALL);
        ballast.step(POSITIVE_STEP_DIR * steps_to_take);
    }
}


void control_text(){
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "L: %f", current_inputs.left);
    display.drawString(0, 1, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "R: %f", current_inputs.right);
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "B: %f", current_inputs.ballast);
    display.drawString(0, 3, disp_buffer);
}

void control_debug_text(){
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "L: %d", esc_left_drive);
    display.drawString(0, 1, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "R: %d", esc_right_drive);
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "B: %d/%d", ballast_pos_current, ballast_pos_desired);
    display.drawString(0, 3, disp_buffer);
}

#endif