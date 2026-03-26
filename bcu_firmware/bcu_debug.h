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

void debug_status_text(){
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, DISP_HEADER);
  display.setFont(FONT);
  display.drawString(0, 1, "               ");
  display.drawString(0, 2, "-> Press For <-");
  display.drawString(0, 3, "   Error Log   ");
}

//stupid name but it follows convention :/
void debug_debug_text(){
  char disp_buffer[DISP_BUFFER_SIZE];
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, "LOGGED ERRORS");
  display.setFont(FONT);
  display.drawString(0, 1, "(e0=NO LOG)");
  snprintf(disp_buffer, DISP_BUFFER_SIZE, "s%de%d s%de%d", err_log[0][0], err_log[0][1], err_log[1][0], err_log[1][1]);
  display.drawString(0, 2, disp_buffer);
  snprintf(disp_buffer, DISP_BUFFER_SIZE, "s%de%d s%de%d", err_log[2][0], err_log[2][1], err_log[3][0], err_log[3][1]);
  display.drawString(0, 3, disp_buffer);
}

void debug_error_text(){
  char disp_buffer[DISP_BUFFER_SIZE];
  display.setFont(FONT_BOLD);
  display.drawString(0, 0, "!ERROR! Log ");
  display.setFont(FONT);
  snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "s%de%d...", err_log[0][0], err_log[0][1]);
  display.drawString(0, 1, disp_buffer);
  display.drawString(0, 2, "-> Press For <-");
  display.drawString(0, 3, "   Error Log   ");  
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
