#include "simpletools.h"                      

#include "global.h"
#include "decoder.h"
#include "motor.h"

int32_t derp[] = //{1000, -1018, 500, -473, 318, -237, 19, -1500, 800, -1039};
{0,2000};
#define MAX_VEL 2000

void vel_test(){

 motor_t *m_t = create_motor(MOTOR_1_PWM_PIN, MOTOR_1_INA_PIN, MOTOR_1_INB_PIN, create_decoder_t(MOTOR_1_DECODER_PIN_1,MOTOR_1_DECODER_PIN_2), 1, 1);
  int32_t index = 1;
  set_vel(m_t,derp[0]*2);

  for(;;) {

      pause(300);
      print("%cCUR_POS %d \nREQ_POS %d\nPRE_POS %d\nCUR_VEL %f\nREQ_VEL %d\nERR_VAR %d\nDBG_VAR %f\nERR_SUM %f", CLS, m_t->cur_pos, m_t->req_pos, m_t->pre_pos, m_t->cur_vel, m_t->req_vel, m_t->err, m_t->dbg_var, m_t->err_sum); 
    //}    
    if(m_t->err <= 100 && m_t->err >= -100) { 
        set_vel(m_t,derp[index]*2);
      index++;
      if(index == 10) index = 0;
    }  
  }        
}

void pos_test(){
  
 motor_t *m_t = create_motor(MOTOR_1_PWM_PIN, MOTOR_1_INA_PIN, MOTOR_1_INB_PIN, create_decoder_t(MOTOR_1_DECODER_PIN_1,MOTOR_1_DECODER_PIN_2), 1, 1);
 int32_t index = 1;
 set_pos(m_t, derp[0], MAX_VEL);
 for(;;) {
   pause(300);
   print("%cCUR_POS %d \nREQ_POS %d\nERR_POS %d\nCUR_VEL %f\nREQ_VEL %d\nERR_VAR %d\nDBG_VAR %f\nERR_SUM %f", CLS, m_t->cur_pos, m_t->req_pos, m_t->err_pos, m_t->cur_vel, m_t->req_vel, m_t->err, m_t->dbg_var, m_t->err_sum);    
   if(m_t->err_pos <= 3 && m_t->err_pos >= -3) { 
      set_pos(m_t, derp[index], MAX_VEL);
      index++;
      if(index == 2) index = 0;
    }  
  }
} 
  
  
int main() {
 
  pos_test();       
}

