#include "simpletools.h"

#include "motor.h"
#include "decoder.h"

#define MODE_POS 0
#define MODE_VEL 1

#define PID_MULT 1

// @12v
// 3 25 0.0003
double kp_strength = .2 * PID_MULT; //2;
double ki_strength = 5 * PID_MULT; //.5;
double kd_strength = 0.0001 * PID_MULT; //.0002;

double kp_vel_strength = 0;
double ki_vel_strength = 1.5; // 1.5
double kd_vel_strength = 0;

double kv_strength = 100;  // 5

// Sets var value to max if over and min if under
int32_t squash(int32_t var, int32_t max, int32_t min) {   
  if (var > max) var = max; 
  if (var < min) var = min;

  return var;   
}



void direction_signal(uint8_t m1_pin, uint8_t m2_pin, double v) {
  // Sends a signal that controls the direction of the motor
  if (v < 0) { low(m1_pin); high(m2_pin); }
  else { high(m1_pin); low(m2_pin); }    
}  


// Loop
void motor_loop(motor_t *m_t, uint32_t *next, uint32_t loop_time, double dt) {
  
  m_t->cur_pos = m_t->decoder->counter;
  m_t->err_pos = delta_pos(m_t);

  // Only calculate this every x loops for precise calculation
  m_t->cur_vel = (m_t->pre_pos - m_t->cur_pos) / dt;

  double pid_value = 0;
  
  // Current Values 
  if (m_t->mode == MODE_VEL) {
    m_t->err = m_t->cur_vel - m_t->req_vel ;
   
    // SUM 
    m_t->err_sum += m_t->err * dt; 
    

    // Calculate algorithm value
    pid_value = 
    m_t->err * kp_vel_strength + 
    m_t->err_sum * ki_vel_strength + 
    ( (m_t->err - m_t->err_prev) / dt ) * kd_vel_strength;
  }
  
  if (m_t->mode == MODE_POS) {    
    m_t->req_vel = kv_strength * m_t->err_pos;
    m_t->req_vel = squash(m_t->req_vel, m_t->max_vel, -(m_t->max_vel));
    
    m_t->err_vel = m_t->cur_vel - m_t->req_vel;
    
    m_t->err = m_t->err_vel;
    

   // SUM
   if (m_t->err_max > m_t->err && m_t->err_max > -(m_t->err))   // Only add to sum if error is more or less than threshhold
   { 
      m_t->err_sum += m_t->err * dt; 
   }
    
    // Calculate algorithm value
    pid_value = 
    m_t->err * kp_strength + 
    m_t->err_sum * ki_strength + 
    ( (m_t->err - m_t->err_prev) / dt ) * kd_strength;
    
  }        

  


  // Send Direction of motor and Speed 
  direction_signal(m_t->m1_pin, m_t->m2_pin, pid_value);
  pwm_set(m_t->rc_pin, 0, abs(squash(pid_value, 500, -500)*0.2));
 
  
  // Set Previous values
  m_t->err_prev = m_t->err;
  m_t->pre_pos = m_t->cur_pos;

  m_t->dbg_var = pid_value;
  
  *next = waitcnt2(*next, loop_time);
}  

// Always adjusts motor position 
void start_pid_motor(void *args) {
  
  // Start decoders
  
  // Create array
  uint8_t count = 1;
//  motor_t **motors = malloc(count * sizeof(motor_t));
  
  motor_t *m_t = (motor_t *)args;
  
  // Start PWM frequency
  pwm_start(100);

  
  uint32_t freq = 100;
  uint32_t loop_time = 80000000 / freq;
  uint32_t next = CNT + 10 * loop_time;
  double dt = (1.0 / (double)freq);

  // Loop through decoders
  for(;;){
    for (uint8_t i = 0; i < count; i++){
      motor_loop(m_t, &next, loop_time, dt);
    }
  }   
}


// Set current position of the motor
void set_pos(motor_t *m_t,int32_t pos, int32_t max_vel) {
  m_t->err_sum = 0;
   
  m_t->mode = MODE_POS;
  
  m_t->max_vel = max_vel;
  m_t->req_pos = pos;
  m_t->err_sum = 0;
}

void set_vel(motor_t *m_t, int32_t vel) {
  m_t->err_sum = 0;
  
  m_t->mode = MODE_VEL;
  
  m_t->req_vel = vel;
  m_t->err_sum = 0;
}  

// Calculates the difference between current and requested position
int32_t delta_pos(motor_t *m_t){
  
  int32_t delt_pos = m_t->cur_pos - m_t->req_pos;
  return delt_pos;
}

int32_t STACK2 [400];

// Constructor
motor_t* create_motor(uint8_t rc_pin, uint8_t m1_pin, uint8_t m2_pin, decoder_t *d_t, int32_t pos, int32_t vel) {
  // Create
  motor_t *m_t = (motor_t *)malloc(sizeof(motor_t));
  
  // Var assign
  m_t->rc_pin = rc_pin;
  m_t->m1_pin = m1_pin;
  m_t->m2_pin = m2_pin;
  
  set_direction(rc_pin, 1); // Output
  set_direction(m1_pin, 1); // Output
  set_direction(m2_pin, 1); // Output
  
  m_t->err_max = 200;
  
  m_t->decoder = d_t;

  m_t->mode = MODE_POS;

  // Start cores
  cogstart(start_pid_motor, m_t, STACK2, sizeof(STACK2));
  
  // Return
  return m_t;
}
// Destructor
void destroy_motor_t(motor_t *m_t) {
  free(m_t);
}  
