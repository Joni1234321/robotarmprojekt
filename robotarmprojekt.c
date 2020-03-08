/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"   
#include "servo.h"

#include "keyboard.h"
#include "motor.h"
#include "global.h"

void reset_motors();
void set_th1(double angle);
void set_th2(double angle);


double l1 = 163;
double l2 = 148;
double robot_height = 217;                 // From keyboard top to the first servo 217

double key_size = 19.5;                     // Size of key from side to side
double ctr_offset = 6;                      // Offset from button to center 

double y_no_press = 10;                     // Y-position when not pressed down 
double y_yes_press = 20;
double row_keyboard_offset = 153;           // From structure to first row ('1', '2')
double col_keyboard_offset = -37;           // From strcuture to first key from the left (tab, Capslock)

double row_offset[5] = { 0, 9, 12, 5, 9 };  // The offset for each row in mm



void print_key(keyswitch_t key, char name){ 
  print("Name: %c\n", name);
  print("I\t th1: %f  \tI\n", key.th1);
  print("I\t th2: %f  \tI\n", key.th2);
  print("I\t col: %f  \tI\n", key.col);
}

void press_key(char);

void press_keys(char* string){
  for(uint16_t i = 0; string[i] != '\0'; ++i){
    press_key(string[i]);
  }    
}


motor_t *motor_col;  // H bro

void init_motors() {
  motor_col = create_motor(MOTOR_1_PWM_PIN, MOTOR_1_INA_PIN, MOTOR_1_INB_PIN, create_decoder_t(MOTOR_1_DECODER_PIN_1,MOTOR_1_DECODER_PIN_2), 1, 1);
}  

int main()                                    
{
  init_keyboard(l1, l2, robot_height, key_size, ctr_offset, y_no_press, y_yes_press, row_keyboard_offset, col_keyboard_offset, row_offset);
  init_motors();

  press_keys("hej");
  reset_motors();
  
  for(;;) {
  }
}



#define RADIAN_TO_DEGREE 57.307

// DC motor
#define MM_TO_ENCODER 23.647      // Konstant for hver mm 4020 / 170
#define ROTATION_EACH_CYCLE 2400
#define KEYPRESS_MAX_VEL 2000
// TH1
#define TH1_RC_COEFFICIENT  4.45   // Konstant for hver grad
#define TH1_RC_STABLE       1280   // RC værdien, der hvor den er parralel med overfladen
// TH2
#define TH2_RC_COEFFICIENT  10     // Konstant for hver grad
#define TH2_RC_STABLE       2400    // RC værdien, der hvor den er parralel med overfladen

#define TOLERANCE 10

char current_char;

void wait_for_complete(double tol){
  if (current_char == 'p' || current_char == 'q') tol *= 3;
  // BLIV I LØKKEN 
  while(motor_col->err_pos >= tol || motor_col->err_pos <= -tol){
    usleep(10);
  }
}

// SET SERVO POSITIONS
void set_th1(double angle){
  double degrees = angle * RADIAN_TO_DEGREE;
  double rc_val = TH1_RC_STABLE + TH1_RC_COEFFICIENT * (degrees);
  servo_set(SERVO_TH1, rc_val);
}
void set_th2(double angle){
  double degrees = angle * RADIAN_TO_DEGREE;
  double rc_val = TH2_RC_STABLE - TH2_RC_COEFFICIENT * (180 - degrees);
  servo_set(SERVO_TH2, rc_val);
}


// Presses the selected key
void press_key(char c) {
  current_char = c;
  keyswitch_t key;

  get_key(&key, c);
  set_pos(motor_col, MM_TO_ENCODER * key.col, KEYPRESS_MAX_VEL);
  
  // Reset Servo motors
  servo_set(SERVO_TH1, 1180);
  pause(100);
  servo_set(SERVO_TH2, 1900);
  pause(200);
  servo_stop();
  // Stop resetting when the col motor, is close to its destination
  wait_for_complete(100);
  
  // Set the Servo motors
  set_th1(key.th1);
  set_th2(key.th2);
  wait_for_complete(TOLERANCE);
  
  // After approx 2.4 seconds, the servo motors have reached its angle, and can return
  pause(2400);
  //print(" ... DONE\n");
}  


void reset_motors(){
  servo_set(SERVO_TH1, 1280);
  pause(100);
  set_pos(motor_col, 0, 2000);
  pause(10000);
  servo_stop();
}  




// Example Code

void ex_1 () {
  
  keyswitch_t key_a;
  get_key(&key_a, 'a');
 
  // The data for the key a is now stored in the variable key_a
  // key_a.th1 is the first angle
  // key_a.th2 is the second angle
  // key_a.col is the position
  
}  

