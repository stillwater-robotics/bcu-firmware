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

const char main_string_0[] PROGMEM = "---------------";
const char main_string_1[] PROGMEM = "Still Water...";
const char main_string_2[] PROGMEM = "Release Button.";

const char * const main_str_table[] PROGMEM {
  main_string_0,
  main_string_1,
  main_string_2
};

void main_page_status_text(){
  // char disp_buffer[DISP_BUFFER_SIZE];
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(main_str_table[0])));
  display.drawString(0, 1, disp_buffer);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(main_str_table[1])));
  display.drawString(0, 2, disp_buffer);
}

void main_page_debug_text(){
  // char disp_buffer[DISP_BUFFER_SIZE];
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(main_str_table[0])));
  display.drawString(0, 1, disp_buffer);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(main_str_table[2])));
  display.drawString(0, 2, disp_buffer);
}

#endif
