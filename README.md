# RC Remote Switch Extender

This is a neat little Arduino sketch made for RC remotes that only have 3-way switches.
With a Arduino Nano and a little bit of wire you can easily turn two 3-way switches into a virtual 9-way switch with easily customizeable PWM
levels for these 9 values.

## Cabling & Co.

![Wiring diagram](https://github.com/olewehrmeyer/rc-switch-extender/raw/master/images/wiring_bb.png "Wiring Diagram")

## How to configure

Use the `read-pwm-levels` sketch first to determine the pwm levels and tolerance needed for your RC remote.
- Connect the ground of the remote receiver to a ground of your Arduino. Always do this first!!
- Connect the PWM channel of the remote receiver to digital pin 2 of your Arduino (or change mapping below)
- Start the sketch and connect a serial monitor with baud 115200
- For each position of the switch
-- Bring switch into that position
-- Note down the average value and recommended range
- Insert the average values and the max(recommended range) into the extender script
- If your values have sufficient spacing, feel free to extend the Tolerance to be sure that the fallback pwm level does not get activated
- Set your wanted fallback pwm level in the extender script
- Compile & upload extender script
- Have fun with your 9-way switch

## Notes

- Set the fallback level to a PWM level that triggers some kind of failsafe mode for your drone
- For power supply, sacrifice an old USB cable to get a better protection for your arduino. Applying 5V directly to the respective pin is not recommended as an overvoltage may easily short out your board
  
