/*
   Use this program to determine what PWM pulse legth your remote sends at different switch positions.

   HOW TO USE:
    - Connect the ground of the remote receiver to a ground of your Arduino. Always do this first!!
    - Connect the PWM channel of the remote receiver to digital pin 2 of your Arduino (or change mapping below)
    - Start the sketch and connect a serial monitor with baud 115200
    - For each position of the switch
      - Bring switch into that position
      - Note down the average value and recommended range
    - Insert the average values and the max(recommended range) into the extender script
*/



#include <Servo.h>

// defines the digital input pins to use
// on Arduino Nano this can only be PIN 2 & 3
#define CHANNEL_IN_PIN 2

// default output pin to emulate PWM signal. Ignore if using a real remote
#define OUTPUT_PIN 9


volatile unsigned long timer_start;

// to calculate average
// increasing size will increase accuracy until calculation causes int overflow
#define ARR_SIZE 10
volatile int last_values[ARR_SIZE];
volatile int curr_arr_pointer = 0;

// for debugging only
Servo output;

void calcSignal()
{
  if (digitalRead(CHANNEL_IN_PIN) == HIGH)
  {
    // pin is high, initialize time counter
    timer_start = micros();
  }
  else
  {
    //only worry about this if the timer has actually started
    if (timer_start != 0)
    {
      //record the pulse time
      last_values[curr_arr_pointer] = ((volatile int)micros() - timer_start);
      curr_arr_pointer = (curr_arr_pointer + 1) % ARR_SIZE;
      //restart the timer
      timer_start = 0;
    }
  }
}

//this is all normal arduino stuff
void setup()
{
  timer_start = 0;
  attachInterrupt(digitalPinToInterrupt(CHANNEL_IN_PIN), calcSignal, CHANGE);
  Serial.begin(115200);

  // debug mode, connect OUTPUT_PIN to CHANNEL_IN_PIN to measure this if no remote receiver at hand
  output.attach(OUTPUT_PIN);
  output.writeMicroseconds(1800);
}

void loop()
{
  delay(1000);
  int sum = 0;
  int minimum = 10000; // okay as remote range should be 1000-2000
  int maximum = 0;
  for (int i = 0; i < ARR_SIZE; i++)
  {
    sum += last_values[i];
    minimum = min(minimum, last_values[i]);
    maximum = max(maximum, last_values[i]);
  }
  Serial.println("=======================");
  Serial.println("Pulse length average:");
  Serial.println(sum / ARR_SIZE);
  Serial.println("Pulse length min:");
  Serial.println(minimum);
  Serial.println("Pulse length max:");
  Serial.println(maximum);
  Serial.println("Recommended range:");
  Serial.println((maximum - minimum) * 2);

}
