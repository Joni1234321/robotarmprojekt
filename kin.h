#ifndef __KIN_H__
#define __KIN_H__



typedef struct {
  double x;
  double y;
} coord_t;

typedef struct {
  double th1;
  double th2;
} angle_t;    


// Returns coordinates based on angles
int32_t forward (coord_t*, angle_t, double, double);
// Returns angles based on coordinates
int32_t inverse (angle_t*, coord_t, double, double);  

#endif
