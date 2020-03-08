#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


int8_t get_key_index (char keyname, uint8_t* row, uint8_t* column);


typedef struct {
  double th1; // TH1 is the first angle of the motor
  double th2; // TH2 is the second angle of the motor
  double col; // Col is the row motors value
} keyswitch_t;

double keyswitch_values[900]; // 3 * sizeof(double) *
void init_keyboard (); 
int32_t get_key(keyswitch_t* k_t, uint32_t key_name);

#endif