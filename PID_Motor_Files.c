#include "simpletools.h"                      

#include "global.h"
#include "decoder.h"
#include "motor.h"

int32_t derp[10] = {1340, 318, 2372, 473, 2318, 237, 19, 2838, 2372, 1039};

int main() {
//  cogstart(start_decoders, NULL, STACK1, sizeof(STACK1));   // Start decoder
//  cogstart(start_pid_motor, NULL, STACK2, sizeof(STACK2));     // Start start motor calculation difference yes
  
  motor_t *m_t = create_motor(MOTOR_1_RC_PIN, create_decoder_t(MOTOR_1_DECODER_PIN_1,MOTOR_1_DECODER_PIN_2));
  int32_t index = 1;
  set_pos(m_t, derp[0]);

  for(;;) {
    //for (uint8_t i = 0; i < 30; i++){

      //set_pos(3283);
      pause(300);
      print("%cCUR_POS %d \nREQ_POS %d\nERR_VAR %d\nDBG_VAR %d\nERR_SUM %f", CLS, m_t->cur_pos, m_t->req_pos, m_t->err, m_t->dbg_var, m_t->err_sum); 
    //}    
    if(m_t->err == 0) { 
      //set_pos(m_t, derp[index]);
      //index++;
      if(index == 10) index = 0;
    }    

   
    
  }    
}

