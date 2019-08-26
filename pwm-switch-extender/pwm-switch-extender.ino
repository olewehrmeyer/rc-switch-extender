// defines the digital input pins to use
// on Arduino Nano this can only be PIN 2 & 3
#define CHANNEL_1_PIN       2
#define CHANNEL_2_PIN       3

// defines switch pwm levels. Measure with read-pwm-levels
#define SWITCH_1_LOW        1100
#define SWITCH_1_NEUTRAL    1500
#define SWITCH_1_HIGH       1900

#define SWITCH_2_LOW        1100
#define SWITCH_2_NEUTRAL    1500
#define SWITCH_2_HIGH       1900

// 2d array with output pwm levels
// structure is: [0] -> low; [1] -> neutral, [2] -> high
// "outer" array is switch 1, "inner" is switch 2
#define OUT_LEVELS {{1000, 1100, 1200},{1400,1500,1600},{1800, 1900, 2000}}

// level if no switch position is detected
#define FALLBACK_LEVEL      1000

// input tolerance +- for switch position detection. Measure with read-pwm-levels
#define TOLERANCE           100
// default output pins for debug mode. 5,6,9, 10,11 should work
#define SIGNAL_OUTPUT_PIN   9

// default output pins for debug mode. 5,6,9, 10,11 should work
#define DBG_OUTPUT_1_PIN    10
#define DBG_OUTPUT_2_PIN    11

// no need to configure anything below this comment
//--------------------------------------------------------

#include <Servo.h>

// pwm calc variables pin 1
volatile unsigned long pin1_timer_start;
volatile int pin1_pulse_length;

// pwm calc variables pin 2
volatile unsigned long pin2_timer_start;
volatile int pin2_pulse_length;

Servo signalOutput;
unsigned int last_output = FALLBACK_LEVEL;

int switchLevels[2][3] = {{SWITCH_1_LOW, SWITCH_1_NEUTRAL, SWITCH_1_HIGH}, {SWITCH_2_LOW, SWITCH_2_NEUTRAL, SWITCH_2_HIGH}};

int mappings[3][3] = OUT_LEVELS;

// debug outputs
Servo output1;
Servo output2;

// log buffer
char logString[130];


void calcSignalPin1()
{

  //if the pin has gone HIGH, record the microseconds since the Arduino started up
  if (digitalRead(CHANNEL_1_PIN) == HIGH)
  {
    pin1_timer_start = micros();
  }
  //otherwise, the pin has gone LOW
  else
  {
    //only worry about this if the timer has actually started
    if (pin1_timer_start != 0)
    {
      //record the pulse time
      pin1_pulse_length = ((volatile int)micros() - pin1_timer_start);
      //restart the timer
      pin1_timer_start = 0;
    }
  }
}

void calcSignalPin2()
{
  //if the pin has gone HIGH, record the microseconds since the Arduino started up
  if (digitalRead(CHANNEL_2_PIN) == HIGH)
  {
    pin2_timer_start = micros();
  }
  //otherwise, the pin has gone LOW
  else
  {
    //only worry about this if the timer has actually started
    if (pin2_timer_start != 0)
    {
      //record the pulse time
      pin2_pulse_length = ((volatile int)micros() - pin2_timer_start);
      //restart the timer
      pin2_timer_start = 0;
    }
  }
}

//this is all normal arduino stuff
void setup()
{
  pin1_timer_start = 0;
  pin2_timer_start = 0;
  pin1_pulse_length = 0;
  pin2_pulse_length = 0;
  attachInterrupt(digitalPinToInterrupt(CHANNEL_1_PIN), calcSignalPin1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANNEL_2_PIN), calcSignalPin2, CHANGE);
  Serial.begin(115200);
  output1.attach(DBG_OUTPUT_1_PIN);
  output1.writeMicroseconds(1500);
  output2.attach(DBG_OUTPUT_2_PIN);
  output2.writeMicroseconds(1100);

  signalOutput.attach(SIGNAL_OUTPUT_PIN);
  signalOutput.writeMicroseconds(FALLBACK_LEVEL);
  Serial.println("Started!");
  delay(500);
}

bool in_tolerace_range(int val, int cmp)
{
  return val >= cmp - TOLERANCE && val <= cmp + TOLERANCE;
}


int getSwitchPosition(int sw, int pl)
{
  for (int i = 0; i < 3; i++)
  {
    if (in_tolerace_range(pl, switchLevels[sw][i]))
    {
      return i;
    }
  }
  return -1;
}


void loop()
{
  // Serial.println(pin1_pulse_length);
  // Serial.println(pin2_pulse_length);
  int pos1 = getSwitchPosition(0, pin1_pulse_length);
  int pos2 = getSwitchPosition(1, pin2_pulse_length);
  int out_ms = (pos1 == -1 || pos2 == -1) ? FALLBACK_LEVEL : mappings[pos1][pos2];
  // sprintf(logString, "Switch 1: %d   Switch 2: %d    Output: %d", pos1, pos2, out_ms);
  // Serial.println(logString);
  if (out_ms != last_output)
  {
    signalOutput.writeMicroseconds(out_ms);
    last_output = out_ms;
    sprintf(logString, "Switch 1: %d (%d)   Switch 2: %d (%d)   Output: %d", pos1, pin1_pulse_length, pos2, pin2_pulse_length, out_ms);
    Serial.println(logString);
  }
  delay(20);
}
