#include "base-internal-com-api/bica.h"

#define PIN_LED_DEBUG 13
#define PIN_LED_SEND 12
#define PIN_BUTTON 11

int button_data = LOW;
byte message_count = 1;
#define MESSAGE_DATA_LEN 9
byte message_data[] = {0xFF, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0};
void setup() {
  Serial.begin(9600);
  while(!Serial);
    
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(LED_SEND, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  digitalWrite(LED_DEBUG, LOW);
  digitalWrite(LED_SEND, LOW);
  delay(900);
  button_data = digitalRead(BUTTON);
  digitalWrite(LED_DEBUG, HIGH);
  
  if(button_data == HIGH){
    while(Serial.availableForWrite() <= MESSAGE_DATA_LEN) delay(10);
    message_data[MESSAGE_DATA_LEN-1] = message_count++;
    digitalWrite(LED_SEND, button_data);
    Serial.write(message_data, MESSAGE_DATA_LEN);
  }
  delay(100);
}
