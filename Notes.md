# Controller - Analog Read

int analogReadOpta(device,pin,update)
return the analog value of the 'pin' for the 'device'.
If update is true the value is actually read from the expansion using an I2C
transaction, otherwise the local copy is of the last value is returned.
When 'update' is true the analog value is updated only for "that" pin.

To update all the analog input at once it is possible to use the function
updateAnalogsIn() that updates only the Analog Inputs, or updateAnalogs() that
updates both analog inputs (ADC) and analog output (DAC or PWM)

On Analog Expansion

# About PIN definition

Pin can be accessed as "channels" (this just means that for analog device
purpose there is not really a single pin function such as "digital input" but 2
pin are almost always involved).

There are 8 "analog" channels (handled by Analog Device chip).
There are 4 "pwm" channels.

Channels goes from 0 to 11 first 8 are "analog", last 4 are pwm.

# ADC functions

When 50/60 Hz rejection is enable, conversion takes about 50 ms per channel

1. VOLTAGE ADC -> ADC 0-10V, with or without pull-down
2. CURRENT ADC -> ADC -2

# TODO

Analog wave
ADC on top of other functions

