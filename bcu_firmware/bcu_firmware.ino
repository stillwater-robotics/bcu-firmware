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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif
#include "base-internal-com-api/bica.h"
#include "bcu_common.h"
#include "bcu_communication.h"
#include "bcu_control.h"
#include "bcu_sensors.h"
#include "bcu_main_page.h"

/* Display */
Adafruit_SSD1306 display(DISP_WIDTH, DISP_HEIGHT, &Wire, -1);
bool display_status;
int current_screen;

void write_display(){
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
}

/* Subsystem Management */
struct _subsystem{
  int (*setup)();
  int (*loop)();
  int (*set_status_text)();
  int (*set_debug_text)();
  int (*set_error_text)(int);
};

// Register subsystems here in order of processing.
// Subsytems need a setup and loop processing function,
// as well as 3 info setting functions.
// set_status_text should set disp_buffer[][] and potentially debug_led_b.
// set_debug_text should set disp_buffer[][] and potentially debug_leb_a and debug_led_b
// set_error_text should set disp_buffer[][].
#define SUBSYSTEM_COUNT 1
const struct _subsystem subsystem_registry[SUBSYSTEM_COUNT]{
  {main_page_setup, main_page_loop, main_page_status_text, main_page_debug_text, main_page_error_text} // Main Display Page & Display Setup
};

/* Setup */
void setup(){
  // Setup display. Non-Blocking on Failure.
  display_status = display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDRESS);
  debug_led_a = LOW;
  debug_led_b = LOW;
  error_led = LOW;
  current_screen = 0;

  //Loop through setup functions
  for (int i = 0; i<SUBSYSTEM_COUNT; i++){
    int err = subsystem_registry[i].setup();
    if(err != EOK){
      subsystem_registry[i].set_error_text(err);
      write_display();
      while(true) delay(1000);
    }
  }
}


/* Loop */
void loop(){
  int debug = digitalRead(P_DEBUG_BUTTON);

  if(!debug){
    //Normal Processing
    //Setup Display Flags
    bool flag_error = false;
    bool flag_overwrite = ((loops_to_swap <= 0)? true: false);
    int next_screen = 0;
    int err = EOK;
    if(loops_to_swap <= 0)
      loops_to_update = 0;

    //Processing Loop
    for(int i = 0; i++; i < SUBSYSTEM_COUNT){
      err = subsystem_registry[i].loop();
      // Force a screen update on error.
      if(err != EOK)
        loops_to_update = 0;

      // If we should update the screen
      if(loops_to_update == 0){
        // Current Screen Logic
        if(i == current_screen){
          if(loops_to_swap > 0){
            // If not time to swap screens, write
            if(err != EOK) subsystem_registry[i].set_error_text(err);
            else subsystem_registry[i].set_status_text();
            if(err != EOK) flag_error = true;
            flag_overwrite = false;
            next_screen = i;
          }else{
            // If time to swap screens, only hold on if there's an error
            // and we haven't seen one before
            if(err != EOK && !flag_error){
              subsystem_registry[i].set_error_text(err);
              flag_error = true;
              next_screen = i;
            }
            flag_overwrite = true;
          }
        // Non-Current Screen Logic
        }else{
          if(flag_overwrite == true && (err != EOK || flag_error == false) || err != EOK && flag_error == false){
            if(err != EOK) subsystem_registry[i].set_error_text(err);
            else subsystem_registry[i].set_status_text();
            if(err != EOK) flag_error = true;
            flag_overwrite = false;
            next_screen = i;
          }
        }
      }
    }// Processing Loop

    //Alive and Error Lights
    error_led = (flag_error == true)? HIGH: LOW;
    if(loops_to_alive_light <=0){
      debug_led_a = (debug_led_a == LOW)? HIGH: LOW;
      loops_to_alive_light = ((debug_led_a == LOW)? ALIVE_LIGHT_DELAY_OFF: ALIVE_LIGHT_DELAY_ON)/LOOP_DELAY;
    }

    // Cleanup Display Loop Vars
    if(loops_to_swap <= 0)
      loops_to_swap = ((current_screen == 0)? DISPLAY_SWAP_DELAY_MAIN : DISPLAY_SWAP_DELAY) / LOOP_DELAY;
    if(loops_to_update <= 0){
      loops_to_update = DISPLAY_UPDATE_DELAY/LOOP_DELAY;
      write_display();
    }
    loops_to_swap--;
    loops_to_update--;
    loops_to_alive_light--;
    current_screen = next_screen;

  //Debug Enabled
  }else{
    //by default, off so it can be set by debug screen.
    debug_led_a = false;

    //Process debug screen. Do this live each time.
    for(int i = 0; i++; i < SUBSYSTEM_COUNT)
      subsystem_registry[i].loop();
    subsystem_registry[current_screen].set_debug_text();
  }

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

// void setup_pins(){
//   pinMode(BCU_PIN_ERROR_LED, OUTPUT);
//   pinMode(BCU_PIN_DEBUG_BUTTON, INPUT);
// }

// int setup_display(){
//   // attempt to begin the display
//   eflag_disp = eflag_disp & ((display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDRESS))? 0: 0b1);
//   // setup display buffers to nothing
//   for(int i = 0; i < BCU_DISPID_COUNT; i++)
//     snprintf(disp_line2_buffer[i], BCU_DISP_LINE_BUFFER_MAX, "");
//   snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "No Messages");
// }

// void setup_serial(){
//   Serial.begin(BCU_MSG_BAUD_RATE);
//   // Wait for 1 second to initialize Serial
//   for(int i =0; i < 10 && !Serial; i++)
//     delay(100);
//   eflag_serial = !Serial;
// }

// //int setup_safety(){}

// int setup_collision_avoidance(){

// }

// //int setup_body_lights(){}

// /*##### Update and Loop Functions #####*/
// void loop_collision_avoidance(){
//   int readin_forward;
//   int readin_down;

//   readin_forward = analogRead(BCU_PIN_CA_FORWARD);
//   readin_down = analogRead(BCU_PIN_CA_DOWN);

//   // very simple direct override for now. 
//   //TODO May need to implement some form of noise filter (multiple readings in a row?) if becomes a problem.
//   ca_override_flag = 0;
//   ca_override_flag += (readin_forward < CA_THRESHOLD)*CA_MASK_FORWARD;
//   ca_override_flag += (readin_down < CA_THRESHOLD)*CA_MASK_DOWN;
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

// void loop_debug_button(){
//   int debug_button = digitalRead(BCU_PIN_DEBUG_BUTTON) == HIGH;
//   if(debug_button && !debug_button_prev){
//     disp_line2_current = BCU_DISPID_DEBUG;
//     if(have_errors()){
//       snprintf(disp_line2_buffer[BCU_DISPID_DEBUG], BCU_DISP_LINE_BUFFER_MAX, "Debug Clearing Error");
//       clear_errors();
//     }else{
//       snprintf(disp_line2_buffer[BCU_DISPID_DEBUG], BCU_DISP_LINE_BUFFER_MAX, "Debug Pressed"); 
//     }

//   }else if(!debug_button && debug_button_prev){
//     disp_line2_current = BCU_DISPID_SAFETY;
//     disp_line2_count = 0;
//   }
//   debug_button_prev = debug_button;
// }

// void loop_display() {
//   // Clear the display
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   // Line 1
//   display.setCursor(0, 0);
//   display.println(BCU_DISP_HEADER);
//   //Line 2
//   if(disp_line2_current > BCU_DISPID_DEBUG){
//     disp_line2_count++;
//     if(disp_line2_count > BCU_DISP_LINE2_INTERVAL){
//       disp_line2_current++;
//       disp_line2_count = 0;
//     }
//     if(disp_line2_current >= BCU_DISPID_COUNT) 
//       disp_line2_current = BCU_DISPID_SAFETY;
//   }
//   display.setCursor(0, 10);
//   display.println(disp_line2_buffer[disp_line2_current]);
//   //Line 3
//   display.setCursor(0, 20);
//   display.println(String(disp_line3_buffer));
//   // Show text on screen
//   display.display(); 
// }

// void setup(){
//   setup_bica();
//   setup_display();
//   setup_pins();
//   setup_serial();
//   setup_collision_avoidance();
//   eflag_setup = have_errors();
//   update_error_display();
//   // Halt on setup error
//   while(eflag_setup) delay(1000);
// }

// void loop(){
//   loop_process_messages();
//   update_error_display();
//   loop_collision_avoidance();
//   loop_debug_button(); //After error_display
//   loop_display(); //must be last
//   delay(10);
// }
