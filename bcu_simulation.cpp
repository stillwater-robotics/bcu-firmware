/*###############################*/
/* Header Files */
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/*###############################*/
/* Simulated Agent */
// Electrical
#define FUSE_MAX_AMPS 10
bool relay_12V_status = false;
bool relay_5V_status = false;
bool fuse_status = true;


/*###############################*/
/* Rendering Moment */
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* box;
TTF_Font* font;


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

// Pin Setup
void pinMode(int i, int mode){
    printf("Pin %d, set %d\n", i, mode);
}


// Fake Pin Reading
typedef int (*_read_func_t)();
struct _read_table_entry{
    int pin;
    _read_func_t generator;
};

// To add functionality to a pin, increment _PINCOUNT and add a 
// pin id (ints of 1+, or defined A0-A5) and a function pointer to a int (void).
#define _PINCOUNT 1
const struct _read_lookup_table _read_table_entry[_PINCOUNT]{
    {0, NULL}
}

_read_func_t get_read_generator(int i){
    for(int j = 0; j < _PINCOUNT; j++){
        if(_read_lookup_table[j].pin == i) return _read_lookup_table[j].func;
    }
    return NULL;
}

int analogRead(int i){
    return digitalRead(i);
}
int digitalRead(int i){
    _read_func_t generator = get_read_generator(i);
    if(!generator) return 0;
    return generator();
}

class Adafruit_SSD1306 {

};

/*###############################*/
/* Main Function */
#include "bcu_firmware/bcu_firmware.ino"

int main(int argc, char* argv[]){
    setup();
}