#define plot 1
/* Output from plot:
Average analog reading,
Thermistor resistance,
Temperature,
PID output
*/

/**********
 * LCD
 *********/
#include <LiquidCrystal595.h> // include the library code

// initialize the library with the numbers of the interface pins
#define Data 2
#define Latch 3
#define Clock 4

LiquidCrystal595 lcd(Data, Latch, Clock);

#define LCDColumns 16
#define LCDRows 2

/**********
 * Read thermistor
 *********/

#define thermistorPin A0 // which analog pin to connect

#define thermistorNominal 100000 // resistance at 25 degrees C

#define temperatureNominal 25 // temp. for nominal resistance (almost always 25 C)

#define numSamples 5 // how many samples to take and average

#define bCoefficient 3950 // the beta coefficient of the thermistor (usually 3000-4000)

#define seriesResistor 100000 // the value of the resistor in series with the thermistor

#define readDelay 10 // delay between reads of the thermistor

int samples[numSamples];

uint8_t i;
float average;

float steinhart;

/**********
 * PID
 *********/
#include <PID_v1.h>

#define SSRPin 3 // must be a pin with PWM

double setPoint = 100; // define variables we'll be connecting to
double PIDInput;
double PIDOutput;

double aggKp = 4; // aggressive tuning parameters
double aggKi = 0.2;
double aggKd = 1;

double consKp = 1; // conservative tuning parameters
double consKi = 0.05;
double consKd = 0.25;

// double Kp = 2; // Proportional constant
// double Ki = 5; // Integral Constant
// double Kd = 1; // Derivative Constant

PID myPID(&PIDInput, &PIDOutput, &setPoint, consKp, consKi, consKd, DIRECT);

double gap; // the difference between setPoint and actual value

#define aggressiveGap 50