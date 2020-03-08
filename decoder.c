#include "simpletools.h"
#include "decoder.h"

// Starts motors
void start_decoder(void *args){
  decoder_t *d_t = (decoder_t*)args;   

  // Loop through decoders
  for(;;) {
    decoder_loop(d_t);
  }  
}  


// Loop of decoder
void decoder_loop(decoder_t *d_t) {
    
    int32_t array[16] = {0, -1, 1, 1000, 1, 0, 1000, -1, -1, 1000, 0, 1, 1000, 1, -1, 0};

    // Get state
    int32_t curr_state = get_states(d_t->pin_end, d_t->pin_start);
    
    // Get decoder value
    int32_t idx = (d_t->prev_state << 2) + curr_state;
    d_t->counter += array[idx];
    
    // Set previous state
    d_t->prev_state = curr_state;
}  

int32_t STACK1 [400];

// Construct
decoder_t* create_decoder_t(uint8_t pin_start, uint8_t pin_end){
  // Create
  decoder_t *d_t = (decoder_t *)malloc(sizeof(decoder_t));

  // Var assign  
  d_t->pin_start = pin_start;
  d_t->pin_end = pin_end;
  
  set_direction(pin_end,    0);   // Input
  set_direction(pin_start,  0);   // Input
  
  d_t->prev_state = get_states(pin_end, pin_start); 

  // Start cores
  cogstart(start_decoder, d_t, STACK1, sizeof(STACK1));
  
  // Return
  return d_t; 
}  

// Destroy
void destroy_decoder_t(decoder_t *d_t) {
  free(d_t);
}  
