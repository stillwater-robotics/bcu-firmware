#ifndef BCU_MAIN_PAGE
#define BCU_MAIN_PAGE
#include "bcu_common.h"

bool prev_main_page;
int splash_text_index;
#define SPLASH_TEXT_COUNT 11

const char MAIN_PAGE_SPLASH_TEXT[SPLASH_TEXT_COUNT][DISP_BUFFER_SIZE]={
  "Blub Blub....",
  "Hello World!",
  "Cook, Chop, Served",
  "@aquatonomous",
  "Now Waterproof!",
  "As Seen on TV",
  "Utterly Washed",
  "Still Water :(",
  "Incoming Call: JPork",
  "@stulog",
  "Prince like Diana"
};

void main_page_randomize_splash(){
   int new_index= random(SPLASH_TEXT_COUNT);
   splash_text_index = (new_index + ((splash_text_index == new_index)? 1:0))%SPLASH_TEXT_COUNT;
}

int main_page_setup(){
    pinMode(P_DEBUG_LED_A, OUTPUT);
    pinMode(P_DEBUG_LED_B, OUTPUT);
    pinMode(P_ERROR_LED, OUTPUT);
    splash_text_index = 0;
    prev_main_page = false;
    return EOK;
}

int main_page_loop(){
    if(current_screen > 0)
      prev_main_page = false;
    
    return EOK;
}

void main_page_status_text(char disp_buffer[3][DISP_BUFFER_SIZE]){
    if(prev_main_page == false){
      prev_main_page = true;
      main_page_randomize_splash();
    }
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "~~~~~~~~~~~~~~~~~~~~");
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, MAIN_PAGE_SPLASH_TEXT[splash_text_index]);
}

void main_page_debug_text(char disp_buffer [3][DISP_BUFFER_SIZE]){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "MAIN SCREEN DEBUG");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "");
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "<release button>");
    prev_main_page = false;
}

void main_page_error_text(char disp_buffer [3][DISP_BUFFER_SIZE], int err){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "!ERROR! MAINPAGE");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "E:%d", err);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "should be impossible");
}

#endif
