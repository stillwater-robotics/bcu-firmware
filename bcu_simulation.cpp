/*###############################*/
/* Header Files */
#include <stdio.h>

/*###############################*/
/* Simulated Agent */


/*###############################*/
/* Faux-Arduino */

// Analog pin definiions
#define A0 -1
#define A1 -2
#define A2 -3
#define A3 -4
#define A4 -5
#define A5 -6

// Other Constants
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

void pinMode(int i, int mode){
    printf("Pin %d, set %d\n", i, mode);
}

class Adafruit_SSD1306 {

}

/*###############################*/
/* Main Function */
#include "bcu_firmware/dummy.ino"

int main(int argc, char* argv[]){
    setup();
}