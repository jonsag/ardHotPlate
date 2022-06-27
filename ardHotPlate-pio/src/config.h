#define plot 1 // prine values instead of regular serial output
/*
Output from plot:
Set point,
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

// ============================================= Profiles =============================================
// Reflow profile
String states[4] = {"Preheat      ", "Soak   ", "Ramp up", "Reflow "};

int Times_profile[3][4] = {{60, 120, 60, 20},
                           {60, 150, 60, 20},
                           {60, 200, 60, 20}};

int Temps_profile[3][4] = {{150, 165, 190, 190},
                           {150, 165, 250, 250},
                           {150, 165, 230, 230}};
/*
int Temps_profile[3][4] = {{150, 165, 235, 235},
                           {150, 165, 250, 250},
                           {150, 165, 230, 230}};
*/
// ===========================================   Settings   ===========================================
const int timeDelay = 5;
//const int overshoot = 20;
const int hotspotCompensation = 0;
//const int hysteresis = 1;

// ===========================================   Variables   ===========================================
double setPoint;
//int roomTemp;
int currentState;
float timeElapsed = 0;
float timeSinceLastHeat = 0;
int timeLeft = 0;
int totalTimeLeft = 0;
float timeHeating = 0;
//float error;
float measuredTemp;
int profile = 0;

unsigned long currentMillis;
unsigned long startMillis = 0;

// ============================================   States   =============================================
enum reflowStates
{
    HEATING,
    COOL,
    STARTUP
};

reflowStates reflowState = STARTUP;

// =============================================   Pins   =============================================
// Relay
//const int relay_pin = 11;

// Buttons
const int button = 12;

/**********
 * PID
 *********/
#include <PID_v1.h>

#define SSRPin 11 // must be a pin with PWM

double PIDInput;  // define variables we'll be connecting to
double PIDOutput;

/*
double aggKp = 4; // aggressive tuning parameters
double aggKi = 0.2;
double aggKd = 1;

double consKp = 1; // conservative tuning parameters
double consKi = 0.05;
double consKd = 0.25;
*/

double kp = 0.25;
double ki = 0.01;
double kd = 0.5;

double gap; // the difference between setPoint and actual value

#define aggressiveGap 50

/**********
 * PID auto tune
 *********/
#include <PID_AutoTune_v0.h>

byte ATuneModeRemember = 2;


double kpmodel = 1.5;
double taup = 100;

double tuningSetPoint = 80;

double theta[50];

double outputStart = 5;

double aTuneStep = 50;
double aTuneNoise = 1;
double aTuneStartValue = 100;

unsigned int aTuneLookBack = 20;

boolean tuning = false;

unsigned long now;
unsigned long modelTime;
unsigned long serialTime;

PID myPID(&PIDInput, &PIDOutput, &setPoint, kp, ki, kd, DIRECT);

PID_ATune aTune(&PIDInput, &PIDOutput);

boolean useSimulation = false; // set to false to connect to the real world