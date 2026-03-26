#ifndef BCU_MAIN_PAGE
#define BCU_MAIN_PAGE
#include "bcu_common.h"

bool prev_main_page;
int splash_text_index;
#define SPLASH_TEXT_COUNT 4

// Note: buffer size needs to include space for the terminating character!
const char MAIN_PAGE_SPLASH_TEXT[SPLASH_TEXT_COUNT][DISP_BUFFER_SIZE]={
  "Blub Blub.... ",
  "Now Waterproof",
  "Utterly Washed",
  "Still Water..."
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

void main_page_status_text(){
  if(prev_main_page == false){
    prev_main_page = true;
    main_page_randomize_splash();
  }
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  display.drawString(0, 1, "---------------");
  display.drawString(0, 2, MAIN_PAGE_SPLASH_TEXT[splash_text_index]);
}

void main_page_debug_text(){
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  display.drawString(0, 1, "---------------");
  display.drawString(0, 2, "Release Button.");
  prev_main_page = false;
}

void main_page_error_text(){
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, "!ERROR! MAINPAGE");
  display.drawString(0, 2, "this should be");
  display.drawString(0, 3, "impossible.");
}

#endif
