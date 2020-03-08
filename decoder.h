#ifndef __DECODER_H__
#define __DECODER_H__

typedef struct {
  uint8_t pin_start;
  uint8_t pin_end;
  
  int32_t prev_state;  
  int32_t counter;

}decoder_t;

void start_decoder(void *args);                                 // Start cores

decoder_t* create_decoder_t(uint8_t pin_start, uint8_t pin_end); // Construct
void destroy_decoder_t(decoder_t *d_t);                          // Deconstruct

void decoder_loop(decoder_t *d_t);                               // Loop


#endif