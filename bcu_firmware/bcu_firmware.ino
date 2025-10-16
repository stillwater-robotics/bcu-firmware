/*
 * #         __  |__                          #
 * #      __L L_|L L__    Stillwater Robotics #
 * #...[+(____________)          -2025-       #
 * #       C_________/                        #
 *  
 * bcu_firmware.ino
 * Created: Oct 9, 2025
 * Last Edited: Oct 16, 2025
 * 
 * This file represents all firmware used on the agent's BCU.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "base-internal-com-api/bica.h"

/*##### Display #####*/
#define BCU_DISP_HEADER "BCU Firmware     v0.3"
#define BCU_DISP_LINE_BUFFER_MAX 21
#define BCU_DISP_WIDTH 128
#define BCU_DISP_HEIGHT 32

#define BCU_DISPID_COUNT 5

#define BCU_DISPID_ERROR 0
#define BCU_DISPID_DEBUG 1
#define BCU_DISPID_SAFETY 2
#define BCU_DISPID_COLLISIONAVOIDANCE_1 3
#define BCU_DISPID_COLLISIONAVOIDANCE_2 4


Adafruit_SSD1306 display(BCU_DISP_WIDTH, BCU_DISP_HEIGHT, &Wire, -1);
char disp_line2_buffer[BCU_DISPID_COUNT][BCU_DISP_LINE_BUFFER_MAX];
char disp_line3_buffer[BCU_DISP_LINE_BUFFER_MAX];

#define BCU_DISP_LINE2_INTERVAL 100
int disp_line2_current = 2;
int disp_line2_count = 0;

/*##### Serial #####*/
#define BCU_MSG_BAUD_RATE 9600
byte msg_in_buffer[BICA_BUFFER_LEN];
byte msg_out_buffer[BICA_BUFFER_LEN];

/*##### Pins #####*/
#define BCU_PIN_DEBUG_BUTTON 2
#define BCU_PIN_ERROR_LED 13

/*##### Error Flags #####*/
int eflag_bica = 0;
int eflag_disp = 0;
int eflag_serial = 0;
int eflag_setup = 0;

/*##### Debug Button #####*/
int debug_button_prev = false;

/*##### BICA Processing Functions #####*/
void bcu_bica_on_nullptr(unsigned char message_id, int type, int index_found){
  eflag_bica = eflag_bica & 0b10 & (type << 3) & (((int)message_id) << 8);
}

/*##### Helper Functions #####*/
int have_errors(){
  return eflag_bica || eflag_disp || eflag_serial || eflag_setup;
}

void clear_errors(){
  eflag_bica = 0;
  eflag_disp = 0;
  eflag_serial = 0;
  eflag_setup = 0;
}

/*##### Setup Functions #####*/
int setup_bica(){
  bica_on_nullptr = bcu_bica_on_nullptr;
  eflag_bica = 0;
}

void setup_pins(){
  pinMode(BCU_PIN_ERROR_LED, OUTPUT);
  pinMode(BCU_PIN_DEBUG_BUTTON, INPUT);
}

int setup_display(){
  // attempt to begin the display
  eflag_disp = eflag_disp & ((display.begin(SSD1306_SWITCHCAPVCC, 0x3C))? 0: 0b1);
  // setup display buffers to nothing
  for(int i = 0; i < BCU_DISPID_COUNT; i++)
    snprintf(disp_line2_buffer[i], BCU_DISP_LINE_BUFFER_MAX, "");
  snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "No Messages");
}

void setup_serial(){
  Serial.begin(BCU_MSG_BAUD_RATE);
  // Wait for 1 second to initialize Serial
  for(int i =0; i < 10 && !Serial; i++)
    delay(100);
  eflag_serial = !Serial;
}

//int setup_safety(){}

//int setup_collision_avoidance(){}

//int setup_body_lights(){}

/*##### Update and Loop Functions #####*/
void update_error_display(){
  if(have_errors())
    digitalWrite(BCU_PIN_ERROR_LED, HIGH);
  
  if(eflag_setup)
    snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "Setup Error | Halt");
  
  if(eflag_bica){
    snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"BICA Error %x", eflag_bica);
    return;
  }
  if(eflag_disp){
    snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"DISP Error %x", eflag_disp);
    return;
  }
  if(eflag_serial){
    snprintf(disp_line2_buffer[BCU_DISPID_ERROR],BCU_DISP_LINE_BUFFER_MAX,"Serial Error %x", eflag_serial);
    return;
  }
}

void loop_process_messages(){
  if(!Serial){
    snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "Serial Disconnect");
    eflag_serial = eflag_serial & 0b10;
  }else{
    eflag_serial = eflag_serial & (~0b10);
    while(Serial.available() >= BICA_BUFFER_LEN){
      // Read in message
      for(int i = 0; i < BICA_BUFFER_LEN; i++)
        msg_in_buffer[i] = (byte)Serial.read();

      // Display all read in messages
      if(BICA_BUFFER_LEN > 0)
        snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "R%02x ", msg_in_buffer[0]);
      for(int i = 1; i < BICA_BUFFER_LEN; i++)
        snprintf(disp_line3_buffer, BCU_DISP_LINE_BUFFER_MAX, "%s%02x", disp_line3_buffer, msg_in_buffer[i]);
      
      _bica_m_function_ptr func;
      func = bica_get_function(msg_in_buffer[0], BICAT_PROCESS);
      if(func != nullptr && func)
        func(msg_in_buffer, BICA_BUFFER_LEN, nullptr);
    }
  }
}

void loop_debug_button(){
  int debug_button = digitalRead(BCU_PIN_DEBUG_BUTTON) == HIGH;
  if(debug_button && !debug_button_prev){
    disp_line2_current = BCU_DISPID_DEBUG;
    if(have_errors()){
      snprintf(disp_line2_buffer[BCU_DISPID_DEBUG], BCU_DISP_LINE_BUFFER_MAX, "Debug Clearing Error");
      clear_errors();
    }else{
      snprintf(disp_line2_buffer[BCU_DISPID_DEBUG], BCU_DISP_LINE_BUFFER_MAX, "Debug Pressed"); 
    }

  }else if(!debug_button && debug_button_prev){
    disp_line2_current = BCU_DISPID_SAFETY;
    disp_line2_count = 0;
  }
  debug_button_prev = debug_button;
}

void loop_display() {
  // Clear the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // Line 1
  display.setCursor(0, 0);
  display.println(BCU_DISP_HEADER);
  //Line 2
  if(disp_line2_current > BCU_DISPID_DEBUG){
    disp_line2_count++;
    if(disp_line2_count > BCU_DISP_LINE2_INTERVAL){
      disp_line2_current++;
      disp_line2_count = 0;
    }
    if(disp_line2_current >= BCU_DISPID_COUNT) 
      disp_line2_current = BCU_DISPID_SAFETY;
  }
  display.setCursor(0, 10);
  display.println(disp_line2_buffer[disp_line2_current]);
  //Line 3
  display.setCursor(0, 20);
  display.println(String(disp_line3_buffer));
  // Show text on screen
  display.display(); 
}

void setup(){
  setup_bica();
  setup_display();
  setup_pins();
  setup_serial();
  eflag_setup = (eflag_bica || eflag_disp || eflag_serial);
  update_error_display();
  // Halt on setup error
  while(eflag_setup) delay(1000);
}

void loop(){
  loop_process_messages();
  update_error_display();
  loop_debug_button(); //After error_display
  loop_display(); //must be last
  delay(10);
}
