#ifndef BCU_POWER
#define BCU_POWER
#include "bcu_common.h"

int relay_5V, relay_12V;

int power_setup(){
    pinMode(P_RELAY_12V, OUTPUT);
    pinMode(P_RELAY_5V, OUTPUT);
    relay_5V = LOW;
    relay_12V = LOW;
    return EOK;
}

int power_loop(){
    //TODO: Power/Safety Loop
    relay_5V = HIGH;
    relay_12V = HIGH;
    digitalWrite(P_RELAY_5V, relay_5V);
    digitalWrite(P_RELAY_12V, relay_12V);
    return EOK;
}

void power_status_text(char disp_buffer[3][DISP_BUFFER_SIZE]){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "Power System: %s", (relay_5V == HIGH && relay_12V == HIGH)? "ON": "CUT");
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "5V: %c | 12V: %c", (relay_5V == HIGH)? '#':'-', (relay_12V == HIGH)? '#':'-');
}

void power_debug_text(char disp_buffer [3][DISP_BUFFER_SIZE]){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "POWER DEBUG");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "relay_5V: %d", relay_5V);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "relay_12V: %d", relay_12V);
}

void power_error_text(char disp_buffer [3][DISP_BUFFER_SIZE], int err){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "!ERROR! POWER");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "E:%d", err);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "<todo>");
}

#endif
