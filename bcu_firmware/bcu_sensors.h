#ifndef BCU_SENSORS
#define BCU_SENSORS
#include "bcu_common.h"
#include <dht.h>

dht DHT;
#define DHT_MEASUREMENT_INTERVAL 200
int dht_loops_remaining;

int sensor_setup(){
    dht_loops_remaining = 0;
    return EOK;
}

int sensor_loop(){
    //TODO: Setup Collision Avoidance
    if(dht_loops_remaining <=0){
      DHT.read11(P_DHT11);
      dht_loops_remaining = DHT_MEASUREMENT_INTERVAL/LOOP_DELAY;
    }
    dht_loops_remaining--;
    return EOK;
}

void sensor_status_text(char disp_buffer[3][DISP_BUFFER_SIZE]){
    char bufa[10], bufb[10];
    dtostrf(DHT.temperature,0,0,bufa);
    dtostrf(DHT.humidity,0,0,bufb);
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, DISP_HEADER);
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "Safety  <CA HERE>"); //TODO
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "Temp: %sC Humid: %s%%", bufa, bufb);
}

void sensor_debug_text(char disp_buffer [3][DISP_BUFFER_SIZE]){
    char bufa[10], bufb[10];
    dtostrf(DHT.temperature,0,2,bufa);
    dtostrf(DHT.humidity,0,2,bufb);
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "SAFETY DEBUG");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "temp: %s humid:%s", bufa, bufb);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "down: %d forward: %d", 0, 0);
}

void sensor_error_text(char disp_buffer [3][DISP_BUFFER_SIZE], int err){
    snprintf(disp_buffer[0], DISP_BUFFER_SIZE, "!ERROR! SAFETY");
    snprintf(disp_buffer[1], DISP_BUFFER_SIZE, "E:%d", err);
    snprintf(disp_buffer[2], DISP_BUFFER_SIZE, "<todo>");
}

#endif
