//
// automatically generated by spin2cpp v3.4.0-beta on Mon Nov 14 09:24:09 2016
// spin2cpp --ccode --gas s3.spin 
//

// Modified from S2 include code version 2015.07.08
// ---[Constants]----------------------------------------------------------------
#include <stdlib.h>
#include <propeller.h>
#undef clkset
#undef cogid
#undef cogstop
#undef locknew
#undef lockret
#undef lockclr
#undef lockset
#undef waitcnt
#undef waitpeq
#undef waitpne
#define _waitcnt(x) __builtin_propeller_waitcnt((x), 0)
#include "s3.h"

#ifdef __GNUC__
#define INLINE__ static inline
#define Yield__() __asm__ volatile( "" ::: "memory" )
#else
#define INLINE__ static
#define Yield__()
#define waitcnt(n) _waitcnt(n)
#define coginit(id, code, par) _coginit((unsigned)(par)>>2, (unsigned)(code)>>2, id)
#define cognew(code, par) coginit(0x8, (code), (par))
#define cogstop(i) _cogstop(i)
#endif

INLINE__ int32_t Min__(int32_t a, int32_t b) { return a < b ? a : b; }
INLINE__ int32_t Max__(int32_t a, int32_t b) { return a > b ? a : b; }
INLINE__ int32_t Shr__(uint32_t a, uint32_t b) { return (a>>b); }
static uint32_t LFSR__(uint32_t x, uint32_t forward) {
    uint32_t y, c, a;
    if (x < 1) x = 1;
    a = forward ? 0x8000000B : 0x17;
    for (y = 0; y < 32; y++) {
       c = __builtin_parity(x & a);
       if (forward) x = (x<<1) | c;
       else         x = (x>>1) | (c<<31);
    }
    return x;
}
#define RandForw__(x) ((x) = LFSR__((x), 1))
#define RandBack__(x) ((x) = LFSR__((x), 0))
typedef void (*Cogfunc__)(void *a, void *b, void *c, void *d);
static void Cogstub__(void *argp) {
  void **arg = (void **)argp;
  Cogfunc__ func = (Cogfunc__)(arg[0]);
  func(arg[1], arg[2], arg[3], arg[4]);
}
__asm__(".global _cogstart\n"); // force clone_cog to link if it is present
extern void _clone_cog(void *tmp) __attribute__((weak));
extern long _load_start_kernel[] __attribute__((weak));
static int32_t Coginit__(int cogid, void *stackbase, size_t stacksize, void *func, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) {
    void *tmp = _load_start_kernel;
    unsigned int *sp = ((unsigned int *)stackbase) + stacksize/4;
    static int32_t cogargs__[5];
    int r;
    cogargs__[0] = (int32_t) func;
    cogargs__[1] = arg1;
    cogargs__[2] = arg2;
    cogargs__[3] = arg3;
    cogargs__[4] = arg4;
    if (_clone_cog) {
        tmp = __builtin_alloca(1984);
        _clone_cog(tmp);
    }
    *--sp = 0;
    *--sp = (unsigned int)cogargs__;
    *--sp = (unsigned int)Cogstub__;
    r = coginit(cogid, tmp, sp);
    return r;
}

static  void s3_obstacler();
static  int32_t s3_faultMonitor();
static  int32_t s3_readBarWidth();

s3 S3;
s3 *self;

void s3_setup()
{
  self = &S3;
  scribbler_start(&self->Scribbler);
  scribbler_start_motors();
  scribbler_start_tones(&self->Scribbler);
  scribbler_button_mode(-1, -1);
  scribbler_set_volume(S3_SPKR_VOL);
  scribbler_set_voices(SCRIBBLER_SAW, SCRIBBLER_SAW);
  self->RunningCog = __builtin_propeller_cogid() + 1;
  Coginit__(30, (void *)self->FMStack, 200, (void *)s3_faultMonitor, 0, 0, 0, 0);
  Coginit__(30, (void *)self->stack, 120, (void *)s3_obstacler, 0, 0, 0, 0);
  _waitcnt((CNT + 10000000));
}

void s3_enableMic()
{
  scribbler_start_mic_env(&self->Scribbler);
}

int32_t s3_readMic()
{
  int micVol = scribbler_get_mic_env();
  micVol -= 350000;
  micVol /= 16500;
  if(micVol < 0) micVol = 0;
  if(micVol > 100) micVol = 100;
  return micVol;
}  

static void s3_obstacler()
{
  int32_t	side, ObstacleThld;
  if (scribbler_get_obstacle_threshold() != SCRIBBLER_DEFAULT_OBSTACLE_THLD) {
    ObstacleThld = scribbler_get_obstacle_threshold();
  } else {
    ObstacleThld = S3_OBSTACLE_THLD;
  }
  while (1) {
    for(side = SCRIBBLER_OBS_TX_LEFT; side <= SCRIBBLER_OBS_TX_RIGHT; side = side + 9) {
      FRQA = (14000 * ObstacleThld) + (20607 * (100 - ObstacleThld));
      CTRA = (4 << 26) | side;
      DIRA |= (1 << side);
      _waitcnt((CNT + 24000));
      self->obs[((side == SCRIBBLER_OBS_TX_RIGHT) + 1)] = -(((INA >> SCRIBBLER_OBS_RX) & 0x1) == 0);
      DIRA &= (~(1 << side));
      _waitcnt((CNT + (CLKFREQ / 8)));
    }
  }
}

// ---[Battery and Over-current Monitor Cog]-------------------------------------
static int32_t s3_faultMonitor()
{
  int32_t value = 0;
  value = 65535;
  _waitcnt((CNT + 80000000));
  while (1) {
    value = Min__(value, scribbler_get_adc_results(SCRIBBLER_ADC_VBAT));
    if (value > 0x87) {
      // 7.0V
      scribbler_set_led(SCRIBBLER_POWER, SCRIBBLER_BLUE);
    } else {
      if (value > 0x73) {
        // 6.0V
        scribbler_set_led(SCRIBBLER_POWER, 32);
      } else {
        scribbler_set_led(SCRIBBLER_POWER, SCRIBBLER_BLINK_BLUE);
      }
    }
    if (scribbler_get_adc_results(SCRIBBLER_ADC_IMOT) > 210) {
      __builtin_propeller_cogstop((self->RunningCog - 1));
      self->RunningCog = 0;
      scribbler_stop_now();
      scribbler_set_leds(SCRIBBLER_BLINK_RED, SCRIBBLER_BLINK_RED, SCRIBBLER_BLINK_RED, SCRIBBLER_OFF);
      while (1) {
        Yield__();
      }
    }
  }
  return value;
}

// ---[Set Motor Speeds]---------------------------------------------------------
void s3_motorSet(int32_t LeftVelocity, int32_t RightVelocity, int32_t move_time)
{
  LeftVelocity = Min__((Max__((-255), ((LeftVelocity * 255) / 100))), 255);
  RightVelocity = Min__((Max__((-255), ((RightVelocity * 255) / 100))), 255);
  if (move_time) {
    move_time = Max__(move_time, 1);
  }
  scribbler_wheels_now(LeftVelocity, RightVelocity, move_time);
  self->WasLeftVelocity = LeftVelocity;
  self->WasRightVelocity = RightVelocity;
  if (move_time) {
    self->WasLeftVelocity = 0;
    self->WasRightVelocity = 0;
    scribbler_wait_stop();
  }
  self->WasLeftVelocity = LeftVelocity;
  self->WasRightVelocity = RightVelocity;
}

void s3_motorSetDistance(int32_t left_distance, int32_t right_distance, int32_t max_speed)
{
  left_distance = Min__((Max__((-10000), left_distance)), 10000);
  right_distance = Min__((Max__((-10000), right_distance)), 10000);
  max_speed = Min__((Max__(1, ((max_speed * 15) / 100))), 15);
  scribbler_move_now(left_distance, right_distance, 0, max_speed, 0);
  scribbler_wait_stop();
}

void s3_motorSetRotate(int32_t degrees, int32_t radius, int32_t max_speed)
{
  if (radius >= 0) {
    degrees = -degrees;
  }
  degrees = Min__((Max__((-1080), degrees)), 1080);
  radius = Min__((Max__((-4400), radius)), 4400);
  max_speed = Min__((Max__(1, ((max_speed * 15) / 100))), 15);
  scribbler_set_speed(max_speed);
  scribbler_arc_deg_now(degrees, (-radius));
  scribbler_wait_stop();
}

int32_t s3_motorsMoving()
{
  return scribbler_moving();
}

void s3_simpleDrive(int32_t Direction, int32_t Speed)
{
  int32_t	LeftVelocity, RightVelocity;
  switch(Direction) {
  case S3_STRAIGHT:
    LeftVelocity = RightVelocity = Speed;
    break;
  case S3_SLIGHT_RIGHT:
    LeftVelocity = Speed;
    RightVelocity = (Speed * 3) / 4;
    break;
  case S3_GENTLE_RIGHT:
    LeftVelocity = Speed;
    RightVelocity = Speed / 2;
    break;
  case S3_SHARP_RIGHT:
    LeftVelocity = Speed;
    RightVelocity = Speed / 4;
    break;
  case S3_SLIGHT_LEFT:
    LeftVelocity = (Speed * 3) / 4;
    RightVelocity = Speed;
    break;
  case S3_GENTLE_LEFT:
    LeftVelocity = Speed / 2;
    RightVelocity = Speed;
    break;
  case S3_SHARP_LEFT:
    LeftVelocity = Speed / 4;
    RightVelocity = Speed;
    break;
  }
  scribbler_wheels_now(LeftVelocity, RightVelocity, 0);
  self->WasLeftVelocity = LeftVelocity;
  self->WasRightVelocity = RightVelocity;
}

void s3_simpleSpin(int32_t Angle, int32_t Speed, int32_t Resume)
{
  scribbler_set_speed(Speed);
  scribbler_arc_deg_now((-Angle), 0);
  scribbler_wait_stop();
  if (Resume) {
  }
  scribbler_wheels_now(self->WasLeftVelocity, self->WasRightVelocity, 0);
}

void s3_simpleStop()
{
  scribbler_wheels_now(0, 0, 0);
  self->WasLeftVelocity = 0;
  self->WasRightVelocity = 0;
}

void s3_setVolume(int32_t Volume)
{
  Volume = Min__((Max__(0, Volume)), 100);
  scribbler_set_volume(Volume);
}

void s3_setVoices(int32_t voice1, int32_t voice2)
{
  scribbler_set_voices(voice1, voice2);
}

// ---[Play an Individual Note, Separated by a Short Delay.]---------------------
void s3_playNote(int32_t freq1, int32_t freq2, int32_t duration)
{
  scribbler_play_tone((Max__((duration - 1), 1)), freq1, freq2);
  scribbler_play_tone(1, 0, 0);
  scribbler_wait_sync(0);
}

void s3_simplePlay(int32_t Frequency, int32_t Duration, int32_t Volume)
{
  Volume = Min__((Max__(0, Volume)), 100);
  scribbler_set_volume(Volume);
  scribbler_play_tone((Max__((Duration - 1), 1)), Frequency, 0);
  scribbler_play_tone(1, 0, 0);
  scribbler_wait_sync(0);
}

// ---[Play an Individual Pulse, Followed by a Delay.]---------------------------
/* 
Pub PlayPulse(on_duration, nil, off_duration) | on_freq
  on_duration := on_duration * 2 + 1380
  on_freq := 1_000_000 / on_duration
  on_duration := on_duration / 700 #> 1
  Scribbler.play_tone(on_duration, on_freq, 0)
  if (off_duration)
    Scribbler.play_tone(off_duration, 0, 0)
'---[Read Bar Codes]-----------------------------------------------------------
 */
int32_t s3_readBars()
{
  int32_t	w0, w1, barcount, midwidth, t;
  int32_t result = 0;
  if ((scribbler_line_sensor(SCRIBBLER_LEFT, S3_BAR_THLD)) || (scribbler_line_sensor(SCRIBBLER_RIGHT, S3_BAR_THLD))) {
    return result;
  }
  t = CNT;
  while (!(scribbler_line_sensor(SCRIBBLER_RIGHT, S3_BAR_THLD))) {
    if ((CNT - t) > 80000000) {
      return result;
    }
  }
  if (!((w1 = s3_readBarWidth(self)))) {
    return result;
  }
  if (!((w0 = s3_readBarWidth(self)))) {
    return result;
  }
  midwidth = Shr__((w0 + w1), 1);
  for(barcount = 1; barcount <= 5; barcount++) {
    if (barcount <= 4) {
      result = result << 1;
      if (w0 > midwidth) {
        result |= 0x1;
      }
    }
    if (!((w0 = s3_readBarWidth(self)))) {
      result = 0;
      return result;
    }
  }
  if (result & 0x8) {
    result = (__builtin_propeller_rev(result, 32 - 3)) | 0x8;
  }
  return result;
}

// ---[Read the Width of One Dark Bar Code Bar]----------------------------------
static int32_t s3_readBarWidth()
{
  int32_t	t;
  t = CNT;
  while (scribbler_line_sensor(SCRIBBLER_RIGHT, S3_BAR_THLD)) {
    if ((CNT - t) > 80000000) {
      return 0;
    }
  }
  t = CNT;
  while (!(scribbler_line_sensor(SCRIBBLER_RIGHT, S3_BAR_THLD))) {
    if ((CNT - t) > 80000000) {
      return 0;
    }
  }
  return (CNT - t);
}

// ---[Read Light Sensors]-------------------------------------------------------
int32_t s3_readObstacle(int32_t Side)
{
  if ((Side == SCRIBBLER_LEFT) || (Side == SCRIBBLER_RIGHT)) {
    return self->obs[Side];
  }
  return 0;
}

int32_t s3_simpleObstacle(int32_t Condition, int32_t Location)
{
  int32_t result = 0;
  char obsL, obsR;
  
  if ((Condition == S3_IS) || (Condition == S3_IS_NOT)) {
    obsL = self->obs[S3_LEFT];
    obsR = self->obs[S3_RIGHT];
    
    self->WasObs[S3_LEFT] = obsL;
    self->WasObs[S3_RIGHT] = obsR;
    
  } else if ((Condition == S3_WAS) || (Condition == S3_WAS_NOT)) {
    obsL = self->WasObs[S3_LEFT];
    obsR = self->WasObs[S3_RIGHT];
  }    
  
  switch(Location) {
    case S3_CENTER:
      if (obsL && obsR) result = 1;
      break;
    case S3_LEFT:
      if (obsL && !obsR) result = 1;
      break;
    case S3_RIGHT:
      if (obsR && !obsL) result = 1;
      break;
    case S3_DETECTED:
      if (obsL || obsR) result = 1;
      break;
  }

  if ((Condition == S3_WAS_NOT) || (Condition == S3_IS_NOT)) result = !result;
  
  return result;    
}

int32_t s3_lineSensor(int32_t Side)
{
  if ((Side == S3_LEFT) || (Side == S3_RIGHT)) {
    return scribbler_line_sensor(Side, (-1));
  }
  return 0;
}

int32_t s3_simpleLine(int32_t Condition, int32_t Location, int32_t Color)
{
  int32_t	Position;
  int32_t result = 0;
  if (!(self->RunningCog)) {
    s3_setup(self);
  }
  Position = 0;
  if ((Condition == S3_IS) || (Condition == S3_IS_NOT)) {
    self->WasLine[S3_LEFT] = scribbler_line_sensor(S3_LEFT, (-1));
    self->WasLine[S3_RIGHT] = scribbler_line_sensor(S3_RIGHT, (-1));
  }
  if ((abs((self->WasLine[S3_LEFT] - self->WasLine[S3_RIGHT]))) < 30) {
    // Low difference, not on an edge
    if ((self->WasLine[S3_LEFT] + self->WasLine[S3_RIGHT]) < 60) {
      // Average reading is dark
      if ((Color == S3_BLACK) && ((Location == S3_CENTER) || (Location == S3_DETECTED))) {
        result = 1;
      }
    } else {
      if ((Color == S3_WHITE) && ((Location == S3_CENTER) || (Location == S3_DETECTED))) {
        // Average reading is light
        result = 1;
      }
    }
  } else {
    // Over an edge
    if (Location == S3_DETECTED) {
      result = 1;
    } else {
      if ((self->WasLine[S3_LEFT] > self->WasLine[S3_RIGHT]) && (((Location == S3_LEFT) && (Color == S3_BLACK)) || ((Location == S3_RIGHT) && (Color == S3_WHITE)))) {
        // Left is brighter
          result = 1;
      } else {
        if ((self->WasLine[S3_RIGHT] > self->WasLine[S3_LEFT]) && (((Location == S3_LEFT) && (Color == S3_WHITE)) || ((Location == S3_RIGHT) && (Color == S3_BLACK)))) {
          // Right is brighter
          result = 1;
        }
      }
    }
  }
  if ((Condition == S3_IS_NOT) || (Condition == S3_WAS_NOT)) result = !result;

  return result;
}

int32_t s3_lightSensor(int32_t Side)
{
  if (((Side == SCRIBBLER_LEFT) || (Side == SCRIBBLER_CENTER)) || (Side == SCRIBBLER_RIGHT)) {
    return ((scribbler_light_sensor(Side) * 100) / 256);
  }
  return 0;
}

int32_t s3_simpleLight(int32_t Condition, int32_t Location)
{
  int32_t result = 0;
  if ((Condition == S3_IS) || (Condition == S3_IS_NOT)) {
    self->WasLight[S3_LEFT] = scribbler_light_sensor(S3_LEFT);
    self->WasLight[S3_RIGHT] = scribbler_light_sensor(S3_RIGHT);
    self->WasLight[S3_CENTER] = scribbler_light_sensor(S3_CENTER);
  }
  if (((self->WasLight[S3_LEFT] > (self->WasLight[S3_RIGHT] + 50)) && (self->WasLight[S3_LEFT] > (self->WasLight[S3_CENTER] + 50))) || ((self->WasLight[S3_LEFT] > ((self->WasLight[S3_RIGHT] * 3) / 2)) && (self->WasLight[S3_LEFT] > ((self->WasLight[S3_CENTER] * 3) / 2)))) {
    if (Location == S3_LEFT) {
      result = 1;
    }
  } else {
    if (((self->WasLight[S3_RIGHT] > (self->WasLight[S3_LEFT] + 50)) && (self->WasLight[S3_RIGHT] > (self->WasLight[S3_CENTER] + 50))) || ((self->WasLight[S3_RIGHT] > ((self->WasLight[S3_LEFT] * 3) / 2)) && (self->WasLight[S3_RIGHT] > ((self->WasLight[S3_CENTER] * 3) / 2)))) {
      if (Location == S3_RIGHT) {
        result = 1;
      }
    } else {
      if (((self->WasLight[S3_CENTER] > (self->WasLight[S3_LEFT] + 50)) && (self->WasLight[S3_CENTER] > (self->WasLight[S3_RIGHT] + 50))) || ((self->WasLight[S3_CENTER] > ((self->WasLight[S3_LEFT] * 3) / 2)) && (self->WasLight[S3_CENTER] > ((self->WasLight[S3_RIGHT] * 3) / 2)))) {
        if (Location == S3_CENTER) {
          result = 1;
        }
      } else {
        if ((Location == S3_DETECTED) && (((self->WasLight[S3_LEFT] + self->WasLight[S3_CENTER]) + self->WasLight[S3_RIGHT]) > 50)) {
          result = 1;
        }
      }
    }
  }
  if ((Condition == S3_IS_NOT) || (Condition == S3_WAS_NOT)) result = !result;

  return result;
}

void s3_setLED(int32_t LED, int32_t Color)
{
  if (((LED == SCRIBBLER_LEFT) || (LED == SCRIBBLER_CENTER)) || (LED == SCRIBBLER_RIGHT)) {
    switch(Color) {
    case S3_COLOR_FF0000:
    case S3_COLOR_00FF00:
    case S3_COLOR_FF7F00:
    case S3_OFF:
      scribbler_set_led(LED, Color);
      break;
    }
  }
}

int32_t s3_stalled()
{
  return scribbler_stalled();
}

int32_t s3_simpleStalled(int32_t Condition)
{
  switch(Condition) {
  case S3_IS:
    self->WasStalled = scribbler_stalled();
    return self->WasStalled;
    break;
  case S3_IS_NOT:
    self->WasStalled = scribbler_stalled();
    return !(self->WasStalled);
    break;
  case S3_WAS:
    return self->WasStalled;
    break;
  case S3_WAS_NOT:
    return !(self->WasStalled);
    break;
  }
}

int32_t s3_simpleButton(int32_t Condition)
{
  switch(Condition) {
  case S3_IS:
    self->WasButton = scribbler_button_press();
    return self->WasButton;
    break;
  case S3_IS_NOT:
    self->WasButton = scribbler_button_press();
    return !(self->WasButton);
    break;
  case S3_WAS:
    return self->WasButton;
    break;
  case S3_WAS_NOT:
    return !(self->WasButton);
    break;
  }
}

int32_t s3_simpleRandom(int32_t Condition)
{
  switch(Condition) {
  case S3_IS:
    self->WasRandom = s3_booleanRandom(self);
    return self->WasRandom;
    break;
  case S3_IS_NOT:
    self->WasRandom = s3_booleanRandom(self);
    return !(self->WasRandom);
    break;
  case S3_WAS:
    return self->WasRandom;
    break;
  case S3_WAS_NOT:
    return !(self->WasRandom);
    break;
  }
}

int32_t s3_booleanRandom()
{
  if ((RandForw__(self->seed)) & 0x1) {
    return -1;
  }
  return 0;
}

int32_t s3_randomRange(int32_t A, int32_t B)
{
  int32_t	High, Low, Range;
  // Ser High and Low to their repective numbers
  if (A < B) {
    Low = A;
    High = B;
  } else {
    if (A == B) {
      // return if the range is zero
      return A;
    } else {
      /* if A > B */
      Low = B;
      High = A;
    }
  }
  // Calculate the range
  Range = High - Low;
  // and return 0 if the range is too large to calculate
  if (Range < 0) {
    return 0;
  } else {
    if (Range == 2147483647) {
      return (Low + ((RandForw__(self->seed)) & 2147483647));
    }
  }
  return (Low + (((RandForw__(self->seed)) & 2147483647) / (2147483647 / (Range + 1))));
}

int32_t s3_buttonCount()
{
  return scribbler_button_count();
}

int32_t s3_buttonPressed()
{
  return scribbler_button_press();
}

int32_t s3_resetButtonCount()
{
  return scribbler_reset_button_count();
}

int32_t s3_runWithoutResult(int32_t input)
{
  return input;
}

int32_t s3_ping(int32_t Pin)
{
  int32_t	MaxLoops, StartCnt, EndCnt;
  int32_t result = 0;
  if (!((0 <= Pin) && (Pin <= 5))) {
    return 0;
  }
  OUTA |= (1 << Pin);
  DIRA |= (1 << Pin);
  _waitcnt(((CLKFREQ / 200000) + CNT));
  OUTA &= (~(1 << Pin));
  DIRA &= (~(1 << Pin));
  MaxLoops = 80;
  while ((--MaxLoops) && (!((INA >> Pin) & 0x1))) {
    Yield__();
  }
  StartCnt = CNT;
  if (!(MaxLoops)) {
    return 0;
  }
  MaxLoops = 2000;
  while ((--MaxLoops) && ((INA >> Pin) & 0x1)) {
    Yield__();
  }
  EndCnt = CNT;
  if (!(MaxLoops)) {
    return 0;
  }
  _waitcnt((16000 + CNT));
  result = EndCnt - StartCnt;
  if ((result < 9200) || (result > 1480000)) {
    return 0;
  }
  return result;
}