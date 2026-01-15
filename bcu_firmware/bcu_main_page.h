#ifndef BCU_MAIN_PAGE
#define BCU_MAIN_PAGE
#include "bcu_common.h"

int main_page_setup(){
    pinMode(P_DEBUG_LED_A, OUTPUT);
    pinMode(P_DEBUG_LED_B, OUTPUT);
    pinMode(P_ERROR_LED, OUTPUT);
    return EOK;
}

int main_page_loop(){
    return EOK;
}

void main_page_status_text(){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "press debug button");
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "on subsequent screens");
}

void main_page_debug_text(){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "try on a different");
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "screen...");
}

void main_page_error_text(int err){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "!ERROR! MAINPAGE");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "E:%d", err);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "should be impossible");
}

#endif