/********************************************************************/
// Arduino Nano based PWM fan speed control
//
// The fan will be turned as soon as a certain temperature (temp_start) is reached. 
// It will be turned off again as soon as the fan has run a certain time
// period (MIN_TIME_RUNNING) and the temperature is below a threshold
// (temp_stop). This should keep the fan from turning on and off all the time.
//
// This codes is based on codes from the following homepages:
// Homepages last checked: 2019-10-13
// Computation of fan RPM: https://www.hackster.io/Buechner/4-wired-fan-control-pwm-bb29ac
// Running 1-wire bus and reading temperature of DS18B20 temperature sensor: https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
// Running PWM at 25 kHz: https://forum.arduino.cc/index.php?topic=415167.0 (post by dlloyd)
//
// Further homepages that gave valuable input and inspiration:
//
// PWM fan control: https://www.instructables.com/id/Temperature-Control-With-Arduino-and-PWM-Fans/
//
//
// Thank you to everyone helping online and posting there sketches/codes.
/********************************************************************/

#include <OneWire.h> 
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

const float temp_start = 31.0; // When to turn on the fan
const float temp_stop = temp_start - 1.5; // When to turn of the fan
const float temp_max = 38.5;

const int duty_min = 24; // Minimum fan speed (ca. 30%)
const int duty_max = 79; // Maximum fan speed (100%) 

const float m = (duty_max - duty_min ) / (temp_max - temp_start);


#define ONE_WIRE_BUS 2 
#define MIN_TIME_RUNNING 300 // in seconds
#define DELAYTIME 10000 //in milliseconds
#define LED 6 // digital pin

unsigned long time_running = 0;
bool fan_on = false;

int pwmPin     = 3; // digital PWM pin 9
unsigned long time;
unsigned int rpm;
int rpmPin = 4;
String stringRPM;

void setup() {
  // put your setup code here, to run once:
 // start serial port 
 Serial.begin(9600); 
 // Start up the library 
 sensors.begin(); 

 // generate 25kHz PWM pulse rate on Pin 3
 pinMode(pwmPin, OUTPUT);   // OCR2B sets duty cycle
 pwm25kHzBegin();
 digitalWrite(rpmPin, HIGH);   // Starts reading

 pinMode(LED, OUTPUT);

 fan_on = false;
 
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures(); // Send the command to get temperature readings 

  Serial.print(" Requesting temperatures..."); 
  float temp = sensors.getTempCByIndex(0);
  Serial.print("Temperature is: "); 
  Serial.println( temp ); 


  int duty = getDuty( temp );
  // Keep fan running if it is running
  if ( fan_on == true )
    duty = max( duty_min, duty );
 
  Serial.print("Computed duty is: "); 
  Serial.println( duty ); 

  // Stop fan only if 
  // 1. Temperature is below temp_stop
  // 2. The fan has been running
  // 3. The fan has been running long enough
  if ( temp < temp_stop && fan_on == true && time_running > MIN_TIME_RUNNING ) {
    duty = 0;
  }
  
  Serial.print("Effective duty is: "); 
  Serial.println( duty ); 
  pwmDuty( duty ); // 100% (range = 0-79 = 1.25-100%)

  if ( duty > 0 ) {
    fan_on = true;  
    delay(1000);
    time_running += 1;
    Serial.print("RPM is: "); 
    getRPMS();
    Serial.println( stringRPM ); 
    digitalWrite( LED, HIGH ); 
  } else {
    fan_on = false;
    time_running = 0;
    digitalWrite( LED, LOW );  
  }

  Serial.print("Fan is: "); 
  Serial.println( fan_on ? "ON" : "OFF" ); 
  Serial.print("Time fan running is: "); 
  Serial.println( time_running ); 
  
  delay(DELAYTIME); 
  if ( fan_on == true )
    time_running += (DELAYTIME/1000);

  // Reset to prevent overflow
  if ( time_running > MIN_TIME_RUNNING )
    time_running = MIN_TIME_RUNNING+1;
}

void pwm25kHzBegin() {
  TCCR2A = 0;                               // TC2 Control Register A
  TCCR2B = 0;                               // TC2 Control Register B
  TIMSK2 = 0;                               // TC2 Interrupt Mask Register
  TIFR2 = 0;                                // TC2 Interrupt Flag Register
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // OC2B cleared/set on match when up/down counting, fast PWM
  TCCR2B |= (1 << WGM22) | (1 << CS21);     // prescaler 8
  OCR2A = 79;                               // TOP overflow value (Hz)
  OCR2B = 0;
}

void pwmDuty(byte ocrb) {
  OCR2B = ocrb;                             // PWM Width (duty)
}

void getRPMS() {
 time = pulseIn(rpmPin, HIGH);
 rpm = (1000000 * 60) / (time * 4);
 stringRPM = String(rpm);
// if (stringRPM.length() < 5) {
//   Serial.println(rpm, DEC);
// }
}

// Computes duty value depending on temperature. 
// Duty increases linearly for increasing temperature.
int getDuty( float temp ) {
  int duty = m * ( temp - temp_start ) + duty_min;

  // Make sure duty does not get negative
  if ( duty < duty_min ) {
    duty = 0;
  }

  // Make sure duty is not too large
  if ( duty > duty_max ) {
    duty = duty_max;
  }
  return duty;
}
