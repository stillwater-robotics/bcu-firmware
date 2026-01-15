#ifndef BCU_COMMUNICATION
#define BCU_COMMUNICATION
#include "bcu_common.h"
#include "base-internal-com-api/bica.h"
  
/*##### Serial #####*/
    #define BCU_MSG_BAUD_RATE 9600
    byte msg_in_buffer[BICA_BUFFER_LEN];
    byte msg_out_buffer[BICA_BUFFER_LEN];


#endif