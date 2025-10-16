#include "base-internal-com-api/bica.h"

#define PIN_LED_DEBUG 13
#define PIN_LED_SEND 12
#define PIN_BUTTON 11

int button_data = LOW;
byte message_count = 1;
byte message_data[BICA_BUFFER_LEN];

void setup() {
  Serial.begin(9600);
  while(!Serial);

  bica_func func = bica_get_function(BICAM_TEST_DUMMY, BICAT_CREATE);
  func(message_data, BICA_BUFFER_LEN, nullptr);
    
  pinMode(PIN_LED_DEBUG, OUTPUT);
  pinMode(PIN_LED_SEND, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
}

void loop() {
  digitalWrite(PIN_LED_DEBUG, LOW);
  digitalWrite(PIN_LED_SEND, LOW);
  delay(900);
  button_data = digitalRead(PIN_BUTTON);
  digitalWrite(PIN_LED_DEBUG, HIGH);
  
  if(button_data == HIGH){
    while(Serial.availableForWrite() <= BICA_BUFFER_LEN) delay(10);
    message_data[BICA_BUFFER_LEN-1] = message_count++;
    digitalWrite(PIN_LED_SEND, button_data);
    Serial.write(message_data, BICA_BUFFER_LEN);
  }
  delay(100);
}
