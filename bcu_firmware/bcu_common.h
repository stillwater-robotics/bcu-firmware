#ifndef _BCU_COMMON
#define _BCU_COMMON

#include <avr/pgmspace.h>

/* Digital Pinout */
// 0 (RX)
// 1 (TX)
#define P_IN1           2
#define P_IN2           3
#define P_IN3           4
#define P_IN4           5
#define P_DHT11         6
#define P_DEBUG_BUTTON  7
#define P_RELAY_12V     8
#define P_RELAY_5V      9
#define P_ESC_L         10
#define P_ESC_R         11
//#define P_BODY_LED      12
#define P_ERROR_LED     13

/* Analog Pinout */
#define P_DEBUG_LED_A                   A0
#define P_DEBUG_LED_B                   A1
#define P_COLLISION_AVOIDANCE_FORWARD   A2
#define P_COLLISION_AVOIDANCE_DOWN      A3
//A4: SDA (In Use)
//A5: SCL (In Use)
//A6 (Free, Input Only)
//A7 (Free, Input Only)

/* Timing */
#define LOOP_DELAY 10
#define DISPLAY_UPDATE_DELAY 500
#define DISPLAY_SWAP_DELAY 2000
#define DISPLAY_SWAP_DELAY_MAIN 4000
#define ALIVE_LIGHT_DELAY_ON 200
#define ALIVE_LIGHT_DELAY_OFF 800
int loops_to_update, loops_to_swap, loops_to_alive_light;

/* Subsystem Management */
/* Single display function: display handle, bool is_debug, int errno*/
//typedef int (*disp_func)(U8X8_SSD1306_128X32_UNIVISION_HW_I2C, bool, int);

/* Error IDs*/
#define EOK               0x00

#define ESERIAL           0x10
#define ESERIAL_CONNECT   0x11

#define EBICA             0x20
#define EBICA_INIT        0x21
#define EBICA_MSG_UNKNOWN 0x22
#define EBICA_LOOKUP      0x23

#define EDHT              0x30
#define EDHT_CONNECT      0x31
#define EDHT_CHECKSUM     0x32
#define EDHT_TIMEOUT      0x33
#define EDHT_ACK_L        0x35
#define EDHT_ACK_H        0x36

/* Text Update - Shared Variables */
#define DISP_HEADER "BCU         V2.0"
#define DISP_BUFFER_SIZE 17
#define DISP_WIDTH 128
#define DISP_HEIGHT 32
#define DISP_ADDRESS 0x3C
#define DISP_NUM_LINES 4

int debug_led_a, debug_led_b, error_led;
int current_screen;

#ifdef ARDUINO
  #include <U8x8lib.h>
#endif
U8X8_SSD1306_128X32_UNIVISION_HW_I2C display(/* reset=*/ U8X8_PIN_NONE);
#define FONT_BOLD u8x8_font_amstrad_cpc_extended_f
#define FONT u8x8_font_pxplusibmcgathin_f //u8x8_font_5x8_r
#define DISPLAY_TYPE U8X8_SSD1306_128X32_UNIVISION_HW_I2C

bool display_status;
char disp_buffer[DISP_BUFFER_SIZE];

/* BICA Serial Comms */
#include "base-internal-com-api/bica.h"
#define BAUD_RATE 9600
#define SERIAL_CONNECT_WAIT_TIME 2000 //ms


struct _subsystem{
  int (*setup)();
  int (*loop)();
  void (*set_status_text)();
  void (*set_debug_text)();
  void (*set_error_text)();
};

#endif
