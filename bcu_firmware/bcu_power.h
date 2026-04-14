#ifndef BCU_POWER
#define BCU_POWER
#include "bcu_common.h"

int relay_5V, relay_12V;

#define RELAY_ON LOW
#define RELAY_OFF HIGH

int power_setup(){
    pinMode(P_RELAY_12V, OUTPUT);
    pinMode(P_RELAY_5V, OUTPUT);
    relay_5V = RELAY_OFF;
    relay_12V = RELAY_OFF;
    digitalWrite(P_RELAY_5V, relay_5V);
    digitalWrite(P_RELAY_12V, relay_12V);

    delay(200);
    relay_5V = RELAY_ON;
    digitalWrite(P_RELAY_5V, relay_5V);

    delay(200);
    relay_12V = RELAY_ON;
    digitalWrite(P_RELAY_12V, relay_12V);

    return EOK;
}

int power_loop(){
    //TODO: Power/Safety Loop
    digitalWrite(P_RELAY_5V, relay_5V);
    digitalWrite(P_RELAY_12V, relay_12V);
    return EOK;
}

void power_status_text(){
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    display.drawString(0, 1, "Power System");
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "5V: %s", (relay_5V == RELAY_ON)? "ON": "CUT");
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "12V: %s", (relay_12V == RELAY_ON)? "ON": "CUT");
    display.drawString(0, 3, disp_buffer);
}

void power_on_debug(){
    power_status_text();
    if(relay_12V == RELAY_ON)
        relay_12V = RELAY_OFF;
    else
        relay_12V = RELAY_ON;
}

#endif
