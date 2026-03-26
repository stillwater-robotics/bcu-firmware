#ifndef BCU_COMMUNICATION
#define BCU_COMMUNICATION
#include "bcu_common.h"
#include "base-internal-com-api/bica.h"
#include "base-internal-com-api/bica_control/bica_control_bcu.h"
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

int send_callback(unsigned char* buffer, int buffer_len){
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
    int error = bica_init_bcu_control(send_callback, controller_callback_states, controller_callback_inputs);
    if(error != EOK){
        comm_recent_error = EBICA_INIT;
        return EBICA_INIT;
    }

    // Attempt to connect to Serial
    int count = SERIAL_CONNECT_WAIT_TIME;
    while (!Serial && count-- > 0)
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
        _bica_m_function_ptr _func;
        _func = bica_get_function(msg_in_buffer[0], BICAT_PROCESS);
        if(_func != nullptr && _func){
            _func(msg_in_buffer, BICA_BUFFER_LEN, nullptr);
            e_badlookup_id = msg_in_buffer[0];
            e_badlookup_type = BICAT_PROCESS;
        }else{
            comm_recent_error = EBICA_LOOKUP;
            return EBICA_LOOKUP; 
        }
    }
    return comm_recent_error;
}

void communication_status_text(){ //TODO
    char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, DISP_HEADER);
    display.setFont(FONT);
    display.drawString(0, 1, "Power System");
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "5V: %s", (relay_5V == RELAY_ON)? "ON": "CUT");
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "12V: %s", (relay_12V == RELAY_ON)? "ON": "CUT");
    display.drawString(0, 3, disp_buffer);
}

void communication_debug_text(){ //TODO
    char disp_buffer[DISP_BUFFER_SIZE];
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, "POWER DEBUG");
    display.setFont(FONT);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "On State: %s", (RELAY_ON == LOW)?"LOW":"HIGH");
    display.drawString(0, 1, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "relay_5V: %d", relay_5V);
    display.drawString(0, 2, disp_buffer);
    snprintf(disp_buffer, DISP_BUFFER_SIZE, "relay_12V: %d", relay_12V);
    display.drawString(0, 3, disp_buffer);
}

void communication_error_text(){ //TODO
    display.setFont(FONT_BOLD);
    display.drawString(0, 0, "!ERROR! POWER");
    display.drawString(0, 2, "this should be");
    display.drawString(0, 3, "impossible.");
}

#endif