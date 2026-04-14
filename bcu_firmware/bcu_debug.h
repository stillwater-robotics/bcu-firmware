#ifndef BCU_DEBUG
#define BCU_DEBUG
#include "bcu_common.h"

#define LOG_SIZE 4
int err_log[LOG_SIZE][2];

//Debug functions, used mostly for testing.
int debug_setup(){
    for(int i = 0; i < LOG_SIZE; i++){
      err_log[i][0] = 0;
      err_log[i][1] = 0;
    }
    return EOK;
};

int debug_loop(){
    return EOK;
};

const char dbg_string_0[] PROGMEM = "-> Press For <-";
const char dbg_string_1[] PROGMEM = "   Error Log   ";
const char dbg_string_2[] PROGMEM = "LOGGED ERRORS";
const char * const dbg_str_table[] PROGMEM = {
  dbg_string_0,
  dbg_string_1,
  dbg_string_2
};

void debug_status_text(){
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[0])));
  display.drawString(0, 2, disp_buffer);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[1])));
  display.drawString(0, 3, disp_buffer);
}

//stupid name but it follows convention :/
void debug_debug_text(){
  display.setFont(FONT_BOLD);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[2])));
  display.drawString(0, 0, disp_buffer);
  display.setFont(FONT);
  snprintf(disp_buffer, DISP_BUFFER_SIZE, "s%de%d s%de%d", err_log[0][0], err_log[0][1], err_log[1][0], err_log[1][1]);
  display.drawString(0, 2, disp_buffer);
  snprintf(disp_buffer, DISP_BUFFER_SIZE, "s%de%d s%de%d", err_log[2][0], err_log[2][1], err_log[3][0], err_log[3][1]);
  display.drawString(0, 3, disp_buffer);
}

void debug_error_text(){
  display.setFont(FONT_BOLD);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[2])));
  display.drawString(0, 0, disp_buffer);
  display.setFont(FONT);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[0])));
  display.drawString(0, 2, disp_buffer);
  strcpy_P(disp_buffer, (char *)pgm_read_ptr(&(dbg_str_table[1])));
  display.drawString(0, 3, disp_buffer);
}

void log_error(int sys, int err){
  for(int i = LOG_SIZE-1; i > 0; i--){
    err_log[i][0] = err_log[i-1][0];
    err_log[i][1] = err_log[i-1][1];
  }
  err_log[0][0] = sys;
  err_log[0][1] = err;
}

#endif
