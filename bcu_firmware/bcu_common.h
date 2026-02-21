#ifndef _BCU_COMMON
#define _BCU_COMMON

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
#define DISPLAY_UPDATE_DELAY 100
#define DISPLAY_SWAP_DELAY 2000
#define DISPLAY_SWAP_DELAY_MAIN 4000
#define ALIVE_LIGHT_DELAY_ON 200
#define ALIVE_LIGHT_DELAY_OFF 800
int loops_to_update, loops_to_swap, loops_to_alive_light;

/* Error IDs*/
//Other than EOK, look in specific subsystems for their corresponsing errors.
#define EOK         0

/* Text Update - Shared Variables */
#define DISP_HEADER "BCU Firmware    v1.0"
#define DISP_BUFFER_SIZE 21
#define DISP_WIDTH 128
#define DISP_HEIGHT 32
#define DISP_ADDRESS 0x3C
char disp_buffer[3][DISP_BUFFER_SIZE];
int debug_led_a, debug_led_b, error_led;
int current_screen;

/* Serial Comms */
#define BAUD_RATE 9600

#endif
