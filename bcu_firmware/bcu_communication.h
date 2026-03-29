#ifndef BCU_COMMUNICATION
#define BCU_COMMUNICATION
#include "bcu_common.h"
#include "base-internal-com-api/bica.h"
#include "base-internal-com-api/bica_control/bica_control.h"
#include "bcu_control.h"
  
/*##### message buffers #####*/
byte msg_in_buffer[BICA_BUFFER_LEN];
byte msg_out_buffer[BICA_BUFFER_LEN];

int comm_recent_error = 0;
uint8_t e_badlookup_id = 0;
int e_badlookup_type = 0;

void bcu_bica_on_nullptr(unsigned char message_id, int type, int index_found){
    e_badlookup_id = message_id;
    e_badlookup_type = type;
}

int bcu_send_callback(unsigned char* buffer, int buffer_len){
    for (int i = 0; i < BICA_BUFFER_LEN; i++){
        if(i < buffer_len)
            msg_out_buffer[i] = buffer[i];
        else
            msg_out_buffer[i] = 0;

        if(Serial)
            Serial.write(msg_out_buffer[i]);
    }
    return EOK;
}

int communication_setup(){
    // Setup error tracking and bad lookup
    e_badlookup_id = 0;
    e_badlookup_type = 0;
    comm_recent_error = 0;
    bica_on_nullptr = bcu_bica_on_nullptr;
    Serial.begin(BAUD_RATE);

    // Setup comms 
    int error = init_bica_control_bcu(bcu_send_callback, bcu_controller_callback_states, bcu_controller_callback_inputs);
    if(error != EOK){
        comm_recent_error = EBICA_INIT;
        return EBICA_INIT;
    }

    // Attempt to connect to Serial
    int count = SERIAL_CONNECT_WAIT_TIME;
    while (!Serial && --count > 0)
        delay(1);
    if(count < 0){
        comm_recent_error = ESERIAL;
        return ESERIAL;
    }
    return EOK;
}

int communication_loop(){
    if(!Serial){
        comm_recent_error = ESERIAL;
        return ESERIAL;
    }

    if(Serial.available() >= BICA_BUFFER_LEN){
        for(int i = 0; i < BICA_BUFFER_LEN; i++)
            msg_in_buffer[i] = (uint8_t)Serial.read();
        bica_function func;
        func = bica_get_function(msg_in_buffer[0], BICAT_PROCESS);
        if(func != nullptr && func){
            int out = func(msg_in_buffer, BICA_BUFFER_LEN, nullptr);
            if(out != EOK){
                comm_recent_error = out;
                e_badlookup_id = msg_in_buffer[0];
                return out;
            }
        }else{
            comm_recent_error = EBICA_LOOKUP;
            e_badlookup_id = msg_in_buffer[0];
            e_badlookup_type = BICAT_PROCESS;
            return EBICA_LOOKUP; 
        }
    }
    return EOK;
}

// Space-saving measure for strings. 
// This is needed for stability.
const char comm_string_0[] PROGMEM = "Communications";
const char comm_string_1[] PROGMEM = "Latest:";
const char comm_string_2[] PROGMEM = "!ERROR! COMMS";
const char comm_string_3[] PROGMEM = "Serial";
const char comm_string_4[] PROGMEM = "Disconnected";
const char comm_string_5[] PROGMEM = "err=0x10";
const char comm_string_6[] PROGMEM = "BICA Bad Lookup";
const char comm_string_7[] PROGMEM = "err=0x23";
const char comm_string_8[] PROGMEM = "BICA Failed to";
const char comm_string_9[] PROGMEM = "Initialize.";
const char comm_string_10[] PROGMEM = "err=0x21";
const char comm_string_11[] PROGMEM = "BICA Unknown Err";

void communication_status_text(){
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_0));
    display.drawString(0, 1, disp_buffer);
    strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_1));
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "RX: 0x%02x |TX: 0x%02x", msg_in_buffer[0], msg_out_buffer[0]);
    display.drawString(0, 3, disp_buffer);
    
}

void communication_debug_text(){ 
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT);
    for (int i = 0; i < 16 && i < BICA_BUFFER_LEN-3; i+=4){
        snprintf(disp_buffer, DISP_BUFFER_SIZE, "r%02x %02x %02x %02x", msg_in_buffer[i], msg_in_buffer[i+1], msg_in_buffer[i+2], msg_in_buffer[i+3]);
        display.drawString(0, i/4, disp_buffer);
    }
    
}

void communication_error_text(){ 
    // char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_2));
    display.drawString(0, 0, disp_buffer);
    display.setFont(FONT);
    switch(comm_recent_error){
        case ESERIAL:
        case ESERIAL_CONNECT:
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_3));
            display.drawString(0, 1, disp_buffer);
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_4));
            display.drawString(0, 2, disp_buffer);
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_5));
            display.drawString(0, 3, disp_buffer);
        break;
        case EBICA_LOOKUP:
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_6));
            display.drawString(0, 1, disp_buffer);
            snprintf(disp_buffer, DISP_BUFFER_SIZE, "msg 0x%02x t=%c", e_badlookup_id, e_badlookup_type == BICAT_CREATE? 'c': 'p');
            display.drawString(0, 2, disp_buffer);
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_7));
            display.drawString(0, 3, disp_buffer);
        break;
        case EBICA_INIT:
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_8));
            display.drawString(0, 1, disp_buffer);
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_9));
            display.drawString(0, 2, disp_buffer);
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_10));
            display.drawString(0, 3, disp_buffer);
           
        break;
        default:
            strcpy_P(disp_buffer, (char *)pgm_read_ptr(comm_string_11));
            display.drawString(0, 1, disp_buffer);
            snprintf(disp_buffer, DISP_BUFFER_SIZE, "msg=0x%02x", e_badlookup_id);
            display.drawString(0, 2, disp_buffer);
            snprintf(disp_buffer, DISP_BUFFER_SIZE, "err=0x%02x", comm_recent_error);
            display.drawString(0, 3, disp_buffer);
        break;
    }
}

#endif