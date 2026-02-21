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

/* Arduino Libraries */ //34/56
// #define USE_DISPLAY_1 /* Adafruit Library (LEGACY) +36% Program Storage Space, +15% Dynamic Space */ 
#define USE_DISPLAY_2 /* U8x8 Library  (PREFFERED) +17% Program Storage Space, +19% Dynamic Space */ 


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

/* Display */
#ifdef USE_DISPLAY_1
  #ifdef ARDUINO
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>
  #endif
  Adafruit_SSD1306 display(DISP_WIDTH, DISP_HEIGHT, &Wire, -1);
#endif
#ifdef USE_DISPLAY_2
  #ifdef ARDUINO
    #include <U8x8lib.h>
  #endif
  U8X8_SSD1306_128X32_UNIVISION_HW_I2C display(/* reset=*/ U8X8_PIN_NONE);
  #define FONT_BOLD u8x8_font_amstrad_cpc_extended_f
  #define FONT u8x8_font_5x8_r
#endif


bool display_status;

/** write_display
 * @brief pushes the display buffer's text to the adafruit display, and updates led lights.
 * 
 */
void write_display(){
#if defined(USE_DISPLAY_1)
  if(display_status == false){
    digitalWrite(P_ERROR_LED, HIGH);
    digitalWrite(P_DEBUG_LED_B, HIGH);
  }else{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    for(int i = 0; i < 3; i++){
      display.setCursor(0, 10*i);
      display.println(disp_buffer[i]);
    }
    display.display();
    digitalWrite(P_DEBUG_LED_A, debug_led_a);
    digitalWrite(P_DEBUG_LED_B, debug_led_b);
    digitalWrite(P_ERROR_LED, error_led); 
  }
#elif defined(USE_DISPLAY_2)
  if(display_status == false){
    digitalWrite(P_ERROR_LED, HIGH);
    digitalWrite(P_DEBUG_LED_B, HIGH);
  }else{
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, disp_buffer[0]);
    display.setFont(FONT);
    for(int i = 1; i < 3; i++)
      display.drawString(0, i, disp_buffer[i]);
    display.display();
    digitalWrite(P_DEBUG_LED_A, debug_led_a);
    digitalWrite(P_DEBUG_LED_B, debug_led_b);
    digitalWrite(P_ERROR_LED, error_led); 
  }
#else
  digitalWrite(P_DEBUG_LED_A, debug_led_a);
  digitalWrite(P_DEBUG_LED_B, debug_led_b);
  digitalWrite(P_ERROR_LED, error_led); 
#endif
}

/* Subsystem Management */
struct _subsystem{
  int (*setup)();
  int (*loop)();
  int (*set_status_text)(char [3][DISP_BUFFER_SIZE]);
  int (*set_debug_text)(char [3][DISP_BUFFER_SIZE]);
  int (*set_error_text)(char [3][DISP_BUFFER_SIZE], int);
};

// Register subsystems here in order of processing.
// Subsytems need a setup and loop processing function,
// as well as 3 info setting functions.
// set_status_text should set disp_buffer[][] and potentially debug_led_b.
// set_debug_text should set disp_buffer[][] and potentially debug_leb_a and debug_led_b
// set_error_text should set disp_buffer[][].
#define SUBSYSTEM_COUNT 4
const struct _subsystem subsystem_registry[SUBSYSTEM_COUNT]{
  {main_page_setup, main_page_loop, main_page_status_text, main_page_debug_text, main_page_error_text}, // Main Display Page & Display Setup
  {sensor_setup, sensor_loop, sensor_status_text, sensor_debug_text, sensor_error_text}, //Safety/Collision Avoidance System
  {power_setup, power_loop, power_status_text, power_debug_text, power_error_text}, //Power System
  //INSERT MORE HERE
  {debug_setup, debug_loop, debug_status_text, debug_debug_text, debug_error_text} // Debugging System (KEEP AS LAST)
};
#define MAIN_PAGE_SYS_NUM 0
#define SENSOR_SYS_NUM 1
#define POWER_SYS_NUM 2
//INSERT MORE HERE
#define DEBUG_SYS_NUM 3 //SUBSYSTEM_COUNT -1 
int prev_err[SUBSYSTEM_COUNT];

/** Setup
 * @brief Arduino setup function. Initializes all subsystems and freezes on error.
 */
void setup(){
  // Setup display. Non-Blocking on Failure.
#if defined(USE_DISPLAY_1)
  display_status = display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDRESS);
#elif defined(USE_DISPLAY_2)
  display_status = display.begin();
#endif

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
      subsystem_registry[i].set_error_text(disp_buffer, err);
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
  error_led = (err_count > 0)? HIGH: LOW;
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
    subsystem_registry[current_screen].set_debug_text(disp_buffer);
  
  // ERRORS
  }else if(err_count > 0 && loops_to_update <= 0){ 
    int index;
    for (int i = 0; i < SUBSYSTEM_COUNT+1; i++){
      // Find starting (skip ahead one if time to swap)
      index = (i + current_screen + ((loops_to_swap <= 0)?1:0))%SUBSYSTEM_COUNT;
      if(err[index] != EOK){
        next_screen = index;
        subsystem_registry[index].set_error_text(disp_buffer, err[index]);
        break;
      }
    }

  // REGULAR STATUS
  }else if(loops_to_update <= 0){ 
    int index = (current_screen + ((loops_to_swap <= 0)?1:0))%SUBSYSTEM_COUNT;
    next_screen = index;
    subsystem_registry[index].set_status_text(disp_buffer);
  }

  // Display Update Logic
  if(loops_to_update <= 0)
    write_display();

  // Clean up and decrement loop variables
  if(!debug){
    if(loops_to_swap <= 0)
      loops_to_swap = ((next_screen == 0)? DISPLAY_SWAP_DELAY_MAIN : DISPLAY_SWAP_DELAY) / LOOP_DELAY;
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








// /*##### BICA Processing Functions #####*/
// void bcu_bica_on_nullptr(unsigned char message_id, int type, int index_found){
//   eflag_bica = eflag_bica & 0b10 & (type << 3) & (((int)message_id) << 8);
// }

// /*##### Setup Functions #####*/
// int setup_bica(){
//   bica_on_nullptr = bcu_bica_on_nullptr;
//   eflag_bica = 0;
// }

// void update_error_display(){
//   if(have_errors())
//     digitalWrite(BCU_PIN_ERROR_LED, HIGH);
  
//   if(eflag_setup)
//     snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "Setup Error | Halt");
  
//   if(eflag_bica){
//     snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"BICA Error %x", eflag_bica);
//     return;
//   }
//   if(eflag_disp){
//     snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"DISP Error %x", eflag_disp);
//     return;
//   }
//   if(eflag_serial){
//     snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"Serial Error %x", eflag_serial);
//     return;
//   }
// }

// void loop_process_messages(){
//   if(!Serial){
//     snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "Serial Disconnect");
//     eflag_serial = eflag_serial & 0b10;
//   }else{
//     eflag_serial = eflag_serial & (~0b10);
//     while(Serial.available() >= BICA_BUFFER_LEN){
//       // Read in message
//       for(int i = 0; i < BICA_BUFFER_LEN; i++)
//         msg_in_buffer[i] = (byte)Serial.read();

//       // Display all read in messages
//       if(BICA_BUFFER_LEN > 0)
//         snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "R%02x ", msg_in_buffer[0]);
//       for(int i = 1; i < BICA_BUFFER_LEN; i++)
//         snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "%s%02x", disp_line3_buffer, msg_in_buffer[i]);
      
//       _bica_m_function_ptr func;
//       func = bica_get_function(msg_in_buffer[0], BICAT_PROCESS);
//       if(func != nullptr && func)
//         func(msg_in_buffer, BICA_BUFFER_LEN, nullptr);
//     }
//   }
// }
