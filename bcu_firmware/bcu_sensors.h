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

void sensor_status_text(){
    char bufa[10], bufb[10], disp_buffer[DISP_BUFFER_SIZE];
    dtostrf(DHT.temperature,0,0,bufa);
    dtostrf(DHT.humidity,0,0,bufb);

    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Safety: TODO"); //TODO
    display.drawString(0, 1, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Temp: %sC", bufa);
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Humid: %s%%", bufb);
    display.drawString(0, 3, disp_buffer);
}

void sensor_debug_text(){
    char bufa[10], bufb[10], disp_buffer[DISP_BUFFER_SIZE];
    dtostrf(DHT.temperature,0,2,bufa);
    dtostrf(DHT.humidity,0,2,bufb);
    display.setFont(FONT);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Fwd: TODO");
    display.drawString(0, 0, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Dwn: TODO");
    display.drawString(0, 1, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Temp: %sC", bufa);
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "Humid: %s%%", bufb);
    display.drawString(0, 3, disp_buffer);
}

void sensor_error_text(){
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, "!ERROR! SENSORS");
    display.drawString(0, 2, "this should be");
    display.drawString(0, 3, "impossible.");
}

#endif
