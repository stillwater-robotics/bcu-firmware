/*
 * #         __  |__                          #
 * #      __L L_|L L__    Stillwater Robotics #
 * #...[+(____________)          -2025-       #
 * #       C_________/                        #
 *  
 * bcu_firmware.ino
 * Created: Oct 9, 2025
 * Last Edited: Jan 14, 2026
 * 
 * This file represents the core firmware and display/status software used on the agent's BCU.
 */

/* Arduino Libraries */

#ifdef ARDUINO
#include <Wire.h>
#endif

#include "base-internal-com-api/bica.h"
#include "bcu_common.h"
#include "bcu_communication.h"
#include "bcu_control.h"
#include "bcu_sensors.h"
#include "bcu_power.h"
#include "bcu_main_page.h"
#include "bcu_debug.h"

void (*disp_write_func)();
void (*prev_write_func)();
/** write_display
 * @brief pushes the display buffer's text to the adafruit display, and updates led lights.
 * 
 */
void write_display(){

    if(display_status == false){
      digitalWrite(P_ERROR_LED, error_led);
      digitalWrite(P_DEBUG_LED_B, HIGH);
      digitalWrite(P_DEBUG_LED_A, debug_led_a);
    }else{
      if(disp_write_func != prev_write_func)
        display.clear();
      if(disp_write_func != nullptr){
        disp_write_func();
        prev_write_func = disp_write_func;
      }else{
        display.setFont(FONT_BOLD);
        display.drawString(0, 0, DISP_HEADER);
        display.setFont(FONT);
        display.drawString(0, 1, "   --?????--   ");
        display.drawString(0, 2, " no screen set ");
      }
      display.display();
      digitalWrite(P_DEBUG_LED_A, debug_led_a);
      digitalWrite(P_DEBUG_LED_B, debug_led_b);
      digitalWrite(P_ERROR_LED, error_led); 
    }
  
}

/* Subsystem Management */

// Register subsystems here in order of processing.
// Subsytems need a setup and loop processing function,
// as well as 3 info setting functions, which should write to display directly, but not call display.display();
#define SUBSYSTEM_COUNT 6

const struct _subsystem subsystem_registry[SUBSYSTEM_COUNT]{
  {main_page_setup, main_page_loop, main_page_status_text, main_page_debug_text, nullptr}, // Main Display Page & Display Setup
  {sensor_setup, sensor_loop, sensor_status_text, sensor_debug_text, sensor_error_text}, //Safety/Collision Avoidance System
  {power_setup, power_loop, power_status_text, power_on_debug, nullptr}, //Power System
  //INSERT MORE HERE
  {communication_setup, communication_loop, communication_status_text, communication_debug_text, communication_error_text},
  {control_setup, control_loop, control_text, control_debug_text, control_text},
  {debug_setup, debug_loop, debug_status_text, debug_debug_text, debug_error_text} // Debugging System (KEEP AS LAST)
};

#define MAIN_PAGE_SYS_NUM 0
#define SENSOR_SYS_NUM 1
#define POWER_SYS_NUM 2
#define COMM_SYS_NUM 3
#define CONTROL_SYS_NUM 4
//INSERT MORE HERE
#define DEBUG_SYS_NUM (SUBSYSTEM_COUNT -1)
int prev_err[SUBSYSTEM_COUNT];

/** Setup
 * @brief Arduino setup function. Initializes all subsystems and freezes on error.
 */
void setup(){
    Serial.println("Hello");
  // Setup display. Non-Blocking on Failure.
  Wire.begin();
  Wire.beginTransmission(DISP_ADDRESS);
  int error = Wire.endTransmission();
  if(error == 0)
    display_status = display.begin();
  else  
    display_status = false;
  disp_write_func = nullptr;
  prev_write_func = nullptr;

  if(display_status){
    display.clear();
    display.setFont(FONT_BOLD);
    display.drawString(0, 1, "   BOOTING   ");
    display.display();
  }

  //Setup Serial (TO BE MOVED)
  Serial.begin(BAUD_RATE);
  
  // Setup Display States
  debug_led_a = LOW;
  debug_led_b = LOW;
  error_led = LOW;
  current_screen = 0;

  //Loop through setup functions
  for (int i = 0; i<SUBSYSTEM_COUNT; i++){
    int err = subsystem_registry[i].setup();
    if(err != EOK){
      disp_write_func = subsystem_registry[i].set_error_text;
      write_display();
      while(true) delay(1000);
    }
  }

  //Setup loop functions
  loops_to_alive_light = 0;
  loops_to_swap = DISPLAY_SWAP_DELAY_MAIN/LOOP_DELAY;
  loops_to_update = DISPLAY_UPDATE_DELAY/LOOP_DELAY;
}


/** Loop
 * @brief Arduino loop function. Processes all subsystems, then chooses which to render via rotation and priority.
 * Debug Screens -> Error Screens -> Status Screens
 */
void loop(){
  // Process all subsystems, collect errors
  int err[SUBSYSTEM_COUNT];
  int err_count = 0;
  for(int i = 0; i< SUBSYSTEM_COUNT; i++){
    
    err[i] = subsystem_registry[i].loop();
    if(err[i] != EOK){ 
      err_count++;
      if(err[i] != prev_err[i])
        log_error(i, err[i]);
    }
    prev_err[i] = err[i];
  }
  

  // Update alive light (now, so that is can be overwritten by debug screens)
  if(loops_to_alive_light <=0){
    debug_led_a = (debug_led_a == HIGH)? LOW: HIGH;
    loops_to_alive_light = ((debug_led_a == HIGH)? ALIVE_LIGHT_DELAY_ON: ALIVE_LIGHT_DELAY_OFF)/LOOP_DELAY;
  }
  
  // Update Error LED based on err_count
  if(err_count > 0){
    error_led = HIGH;
    err[DEBUG_SYS_NUM] = 1; //Hardcoded so that this displays, even with errors.
  }else{
    error_led = LOW;
  }

  // Get debug state
  bool debug = (digitalRead(P_DEBUG_BUTTON) == HIGH)? true: false;

  // Update screen buffers and LED states
  // Prep logic variables
  int next_screen = current_screen;
  if(loops_to_swap <=0) loops_to_update = 0;

  // DEBUG
  if(debug){ 
    
    disp_write_func = subsystem_registry[current_screen].set_debug_text;
  
  // ERRORS
  }else if(err_count > 0 && loops_to_update <= 0){ 
    
    int index;
    for (int i = 0; i < SUBSYSTEM_COUNT+1; i++){
      // Find starting (skip ahead one if time to swap)
      index = (i + current_screen + ((loops_to_swap <= 0)?1:0))%SUBSYSTEM_COUNT;
      if(err[index] != EOK){
        next_screen = index;
        disp_write_func = subsystem_registry[index].set_error_text;
        break;
      }
    }

  // REGULAR STATUS
  }else if(loops_to_update <= 0){ 
    int index = (current_screen + ((loops_to_swap <= 0)?1:0))%SUBSYSTEM_COUNT;
    next_screen = index;
    disp_write_func = subsystem_registry[index].set_status_text;
  }

  // Display Update Logic
  if(loops_to_update <= 0)
    write_display();
  

  // Clean up and decrement loop variables
  if(!debug){
    if(loops_to_swap <= 0)
      loops_to_swap = ((next_screen == 0 || err_count > 0)? DISPLAY_SWAP_DELAY_MAIN :DISPLAY_SWAP_DELAY) / LOOP_DELAY;
    loops_to_swap--;
  }
  
  if(loops_to_update <= 0)
    loops_to_update = DISPLAY_UPDATE_DELAY/LOOP_DELAY;
  loops_to_update--;
  loops_to_alive_light--;

  // Track 
  current_screen = next_screen;

  // Wait for next loop
  delay(LOOP_DELAY);
}
