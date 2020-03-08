#include "simpletools.h"
#include "kin.h"

// FORWARD RETURNS POSITIONS BASED ON ANGLES
int32_t forward (coord_t* c, angle_t a, double l1, double l2) {
  c->x = ( l1 * cos(a.th1) ) + ( l2 * cos(a.th1 + a.th2) );
  c->y = ( l1 * sin(a.th1) ) + ( l2 * sin(a.th1 + a.th2) );
  return 0; // Completed
}

// INVERSE RETURNS ANGLES IN RADS (CHADS)
int32_t inverse (angle_t* a, coord_t c, double l1, double l2) {
  
  double k0 = (c.x*c.x + c.y*c.y - l1*l1 - l2*l2)/(2 * l1 * l2);
  double v2 = acos(k0);  // Gives us a value in radians 
 
  if (k0 <= -1 || k0 >= 1) {
      return -1; // Failed
  }     
  
  double k1 = (l2 * cos(v2) + l1);  // Save time
  double k2 =  l2 * sin(v2);       // Save time


  double v1 = atan2(
  ( ( c.y * k1 ) - ( c.x * k2 ) ),
  ( ( c.x * k1 ) + ( c.y * k2 ) )
  );
  
  a->th1 = v1;
  a->th2 = v2;
  
  return 0; // Completed
  
}  