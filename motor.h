#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "decoder.h"

typedef struct {
  
  uint8_t rc_pin;
  uint8_t m1_pin;
  uint8_t m2_pin;
  
  int32_t mode;
  
  int32_t req_pos;   // Requested position
  int32_t cur_pos;   // Current position
  int32_t pre_pos;   // Previous position
  int32_t err_pos;   // Error position
  
  
  int32_t req_vel;
  double  cur_vel;
  int32_t err_vel;
  int32_t max_vel;
  
 
  // PID
  int32_t err;       // Error variable
  int32_t err_prev;  // Previous error value
  double  err_sum;   // Sum of errors
  int32_t err_max;   // Values above this var is not added to the sum
  
  double dbg_var;
  
  
  decoder_t *decoder; // Decoder
    
} motor_t;

void start_pid_motor(void *args);                                                                                 // Start cores
motor_t* create_motor(uint8_t rc_pin, uint8_t m1_pin, uint8_t m2_pin, decoder_t *d_t, int32_t pos, int32_t vel);  // Constructor
void destroy_motor_t(motor_t *m_t);                                                                               // Destructor      
   
void motor_loop(motor_t *m_t, uint32_t *next, uint32_t loop_time, double dt );                                   // Loop



void set_pos(motor_t *m_t, int32_t pos, int32_t max_vel);                          // Set position
void set_vel(motor_t *m_t, int32_t vel);                          // Set velocity
int32_t delta_pos(motor_t *m_t);                                  // Get difference between position and wanted position



#endif