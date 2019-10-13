# arduino-nano-fancontrol

A simple PWM fan control based on an Ardiuno nano. 

I use this to cool a the compartment some hardware is running in since ventilation in this compartment is very bad. The fan shall be turned on depending on the temperature. 

## Hardware used

- `Arduino Nano`
- A fan than can be controlled via PWM. 
    - I used a `Noctua NF-F12 5V PWM`. I use the included adapter to power the fan and the Arduino Nano. 
    - **DO NOT** power the fan via the Arduino 5V pin. It will most likely drain way too much energy and kill the Arduino!
- Bread board, some cables and a LED.
- A `DS18B20` temperature sensor. You can use other sensors as well, but then you have to change parts of the code. 

The temperature sensor is connected to digital pin 2 (`ONE_WIRE_BUS`). Don't forget to add a resistor. The led is connected to digital pin 6 (`LED`). The PWM signal is generate on digital pin 3 (`pwmPin`) and the rpm signal of the fan is read on digital pin 4 (`rpmPin`).

The cabling has been done as in tutorials online. For that, please see the section "Acknowledgements".

## Software

The Arduino sketch can be found in `src/`. The code is a bit messy at the moment, but should have sufficient comments.  

The fan will be turned as soon as a certain temperature (`temp_start`) is reached. It will be turned off again as soon as the fan has run a certain time period (`MIN_TIME_RUNNING`) and the temperature is below a threshold (`temp_stop`). This should keep the fan from turning on and off all the time. While the fan is running a led is turned on. 

### Bugs

- After turning the device on the fan spins up for `MIN_TIME_RUNNING` seconds. That was not intended. Maybe I need to add some `delay` after boot up so everything is initialized properly and that the temperature sensor gives sensible temperature readings. 

## Dependencies 

Arduino Libraries:

- `OneWire`
- `DallasTemperature` 

## Acknowledgements

This codes is based on codes from the following homepages (last checked: 2019-10-13):

-  Computation of fan RPM: https://www.hackster.io/Buechner/4-wired-fan-control-pwm-bb29ac
- Running 1-wire bus and reading temperature of DS18B20 temperature sensor: https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
- Running PWM at 25 kHz: https://forum.arduino.cc/index.php?topic=415167.0 (post by dlloyd)

Further homepages that gave valuable input and inspiration:

- PWM fan control: https://www.instructables.com/id/Temperature-Control-With-Arduino-and-PWM-Fans/
