/*
 * #         __  |__                          #
 * #      __L L_|L L__    Stillwater Robotics #
 * #...[+(____________)          -2025-       #
 * #       C_________/                        #
 *  
 * bcu_messaging.h
 * Created: Oct 9, 2025
 * Last Edited: Oct 10, 2025
 * 
 * This file defines messages used in the internal communications system.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "base-internal-com-api/bica.h"

// Display Header/Identifier
#define DISPLAY_HEADER "BCU Firmware     v0.1"
// Display Other Lines
String display_line2_buffer = "";
#define DISPLAY_LINE3_BUFFER_MAX 21
char display_line3_buffer[DISPLAY_LINE3_BUFFER_MAX];
 
// OLED Status Screen Size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
 
// Initialize Adafruit OLED display (I2C address 0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Serial
#define BAUD_RATE 9600

// Define Pins
#define BUTTON_DEBUG 2
#define LED_DEBUG 13

// Global variables (Bad practice, to be fixed)
int display_error = false;
int debug_button = false;
#define MESSAGE_DATA_LEN 9
byte message_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
 
void setup() {
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(BUTTON_DEBUG, INPUT);
 
  // Start OLED display 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    display_error = true;

 //Setup serial and buffer
 Serial.begin(BAUD_RATE);

 //Initialize display info
 snprintf(display_line3_buffer, DISPLAY_LINE3_BUFFER_MAX, "No Messages");
    
}
 
void loop() {
  readDebugButton();
  readInMessages();
  updateDebugDisplay();
  updateErrorLight();
  delay(10);
}

void readInMessages(){
  if(!Serial){
    snprintf(display_line3_buffer, DISPLAY_LINE3_BUFFER_MAX, "Serial Disconnect");
  }else{
    while(Serial.available() >= MESSAGE_DATA_LEN){
      for(int i = 0; i < MESSAGE_DATA_LEN; i++)
        message_data[i] = (byte)Serial.read();
      snprintf(display_line3_buffer, DISPLAY_LINE3_BUFFER_MAX, "R%02x %02x%02x%02x%02x%02x%02x%02x%02x", 
        message_data[0], message_data[1], message_data[2], message_data[3], message_data[4],
        message_data[5], message_data[6], message_data[7], message_data[8]);
    }
  }
}

void readDebugButton(){
  debug_button = digitalRead(BUTTON_DEBUG) == HIGH;
  display_line2_buffer = debug_button? "Debug Button: ON" : "Debug Button: OFF";
}

void updateErrorLight(){
  digitalWrite(LED_DEBUG, (display_error || debug_button)? HIGH:LOW);
}

void updateDebugDisplay() {
  display.clearDisplay();  // Clear buffer
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(DISPLAY_HEADER);
  display.setCursor(0, 10);
  display.println(display_line2_buffer);
  display.setCursor(0, 20);
  display.println(String(display_line3_buffer));
  display.display(); // Show text on screen
}
