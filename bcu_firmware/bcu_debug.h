#ifndef BCU_DEBUG
#define BCU_DEBUG
#include "bcu_common.h"

#define LOG_SIZE 6
int err_log[LOG_SIZE][2];

//Debug functions, used mostly for testing and not included in the final product.
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

void debug_status_text(char disp_buffer[3][DISP_BUFFER_SIZE]){
  snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
  snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "Debug Screen");
  snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "<Press to See Log>");
};

//stupid name but it follows convention :/
void debug_debug_text(char disp_buffer[3][DISP_BUFFER_SIZE]){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "ERRORS (e0=NO LOG)");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "s%de%d s%de%d s%de%d", err_log[0][0], err_log[0][1], err_log[1][0], err_log[1][1], err_log[2][0], err_log[2][1]);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "s%de%d s%de%d s%de%d", err_log[3][0], err_log[3][1], err_log[4][0], err_log[4][1], err_log[5][0], err_log[5][1]);
};

void debug_error_text(char disp_buffer[3][DISP_BUFFER_SIZE], int err){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "!ERROR! Log Page");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "s%de%d...", err_log[0][0], err_log[0][1]);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "<Press for Full Log>");
};

void log_error(int sys, int err){
  for(int i = LOG_SIZE-1; i > 0; i--){
    err_log[i][0] = err_log[i-1][0];
    err_log[i][1] = err_log[i-1][1];
  }
  err_log[0][0] = sys;
  err_log[0][1] = err;
}

#endif
