#include "simpletools.h"
#include "keyboard.h"
#include "kin.h"

// init_keyboard(1, 1, 19, (19/2), 10, 0, 0);

uint8_t layout_rows = 5;
uint8_t layout_columns = 15;
uint8_t layout_size = 75;

// \n is the enter key
// 0x00 not registered ascii value
// 0x1FFFF is caps lock
uint32_t layout [] = {                                                      // ALL VALUES AFTER THE COMMA ARE FILLERS FOR THE ARRAY SO EACH ROW HAS 15 VALUES
  0xC2BD ,'1','2','3','4','5','6','7','8','9','0','+',0xC2B4 ,'\b'          , 0x00 ,
  '\t', 'q','w','e','r','t','y','u','i','o','p',0xC3A5 ,0xC2A8 , '\n'       , 0x00 ,
  0x1FFFF ,'a','s','d','f','g','h','j','k','l',0xC3A6 ,0xC3B8 ,'\'','\n'    , 0x00 , 
  0x00 ,0x3C ,'z','x','c','v','b','n','m', ',' ,'.','-',0x00                , 0x00 , 0x00 ,
  0x00 ,0x00 ,0x00 ,0x00 , 0x00 , 0x00 , ' ' , 0x00 , 0x00 ,0x00 ,0x00      , 0x00 , 0x00 , 0x00 , 0x00 };


void set_key_val(uint8_t key_index, double th1, double th2, double col) {
  keyswitch_values[key_index * 3 + 0] = th1; 
  keyswitch_values[key_index * 3 + 1] = th2;
  keyswitch_values[key_index * 3 + 2] = col;
}  

void generate_key(){
  
}

void init_keyboard(double l1, double l2, double robot_height, double key_size, double ctr_offset, double y_no_press, double y_yes_press, double row_keyboard_offset, double col_keyboard_offset, double row_offset[]) {
  // 3 doubles pr char
  
  for (uint8_t i = 0; i < layout_size; i++){
    // Skip value if 0x00 since it doesnt matter
    if (layout[i] != 0x00){ 
    
      uint8_t col = i % layout_columns;
      uint8_t row = tolower(i / 15);
      
      coord_t c;
      c.x = -(row * key_size) + ctr_offset
      + row_keyboard_offset;
      
      // Tastaturets hældning = 2,5 %
      double slope = row * 2;
      c.y = robot_height - (y_no_press) - slope ;
      angle_t a; 
       
      double col_pos = col * key_size + row_offset[row] + ctr_offset 
      + col_keyboard_offset;
      
      // Inverse the function        
      if (inverse(&a, c, l1, l2) != 0) { 
        print("Error: \t%d\n",i); 
      }
      set_key_val(i, a.th1, a.th2, col_pos);
      // If no results, then error
    }      
  }  
}     





// Returns an error value and a key
int32_t get_key(keyswitch_t* key, uint32_t key_name) {
  //uint32_t search = tolower(key_name);
   
  uint8_t i;
  // Search entire array for character
  for (i = 0; i < layout_size; i++) {

    if (layout[i] == key_name) {
      key->th1 = keyswitch_values[i * 3 + 0]; 
      key->th2 = keyswitch_values[i * 3 + 1];
      key->col = keyswitch_values[i * 3 + 2]; 
      return 0;   
    }       
  }
  
  return -1;    
}  
  
