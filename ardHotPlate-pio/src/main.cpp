#include <Arduino.h>

#include <config.h>
#include <lcd.h>
#include <thermistor.h>

void setup(void)
{
  Serial.begin(9600);
  // analogReference(EXTERNAL);

  lcd.begin(LCDColumns, LCDRows);

  backlightOn(HIGH);

  PIDInput = readThermistor();

  myPID.SetMode(AUTOMATIC); // turn the PID on
}

void loop(void)
{
  PIDInput = readThermistor();

  gap = abs(setPoint - PIDInput); // distance away from set point

  if (gap < aggressiveGap) // we're close to set point, use conservative tuning parameters
  {
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else // we're far from set point, use aggressive tuning parameters
  {
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  myPID.Compute();

  if (!plot)
  {
    Serial.print("\tPID output:  ");
    Serial.println(PIDOutput);
  }
  else
  {
    Serial.println(PIDOutput);
  }

  analogWrite(SSRPin, PIDOutput);
}
