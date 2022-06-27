#include <Arduino.h>

#include <config.h>
#include <lcd.h>
#include <thermistor.h>
#include <autoTune.h>

// ============================================= Setup =============================================
void setup()
{
  Serial.begin(9600);
  // Serial.println("i, setPoint, measured, control, integral, derivative");

  // Compensate for hot spots & Add up the times
  for (int j = 0; j < sizeof(Temps_profile) / sizeof(Temps_profile[0]); j++)
  {
    for (int i = 0; i < sizeof(Temps_profile[0]) / sizeof(Temps_profile[0][0]); i++)
    {
      Temps_profile[j][i] += hotspotCompensation;
    }
    for (int i = 1; i < sizeof(Times_profile[0]) / sizeof(Times_profile[0][0]); i++)
    {
      Times_profile[j][i] += Times_profile[j][i - 1];
    }
  }

  // Pins
  // pinMode(relay_pin, OUTPUT);
  // digitalWrite(relay_pin, LOW);

  pinMode(button, INPUT);

  // LCD
  lcd.begin(LCDColumns, LCDRows);
  backlightOn(HIGH);

  lcd.setCursor(0, 1);
  lcd.print((int)readThermistor());
  lcd.setCursor(3, 1);
  lcd.print("=>");
  lcd.setCursor(8, 1);
  lcd.print((char)223);
  lcd.print("C");

  // PIDInput = readThermistor();

  if (useSimulation)
  {
    for (byte i = 0; i < 50; i++)
    {
      theta[i] = outputStart;
    }
    modelTime = 0;
  }

  myPID.SetMode(AUTOMATIC); // set up the pid

  if (tuning)
  {
    tuning = false;
    changeAutoTune();
    tuning = true;
  }

  serialTime = 0;
}

// ============================================= Loop =============================================
void loop()
{
  currentMillis = millis();
  now = currentMillis;

  if (tuning) // ********** Auto Tune Mode **********
  {
    lcd.setCursor(0, 0);
    lcd.print("Tuning...");

    setPoint = tuningSetPoint;
  }
  else // ********** Normal Mode **********
  {
    switch (reflowState)
    {
    case HEATING:
    {
      setPoint = Temps_profile[profile][currentState];
      timeLeft = Times_profile[profile][currentState] - timeElapsed;
      totalTimeLeft = Times_profile[profile][3] - timeElapsed;

      // LCD
      lcd.setCursor(0, 0);
      lcd.print(states[currentState]);
      lcd.setCursor(9, 0);
      lcd.print("        ");
      lcd.setCursor(9, 0);
      lcd.print(timeLeft);
      lcd.setCursor(13, 0);
      lcd.print(totalTimeLeft);

      if (timeElapsed >= Times_profile[profile][currentState] && measuredTemp >= setPoint)
      {
        currentState++;
        if (currentState == 4)
        {
          reflowState = COOL;
          break;
        }
      }

      if (digitalRead(button) == 1)
      {
        delay(200);
        if (digitalRead(button) == 1)
        {
          reflowState = STARTUP;

          // LCD
          lcd.setCursor(0, 0);
          lcd.print("Reflow stopped  ");

          Serial.println("Reflow Stopped");
          delay(2000);
        }
      }
    }
    break;
    case COOL:
    {
      setPoint = 0.0;

      // LCD
      lcd.setCursor(0, 0);
      lcd.print("Cooling down    ");

      if (measuredTemp < 50)
      {
        reflowState = STARTUP;
      }
    }
    break;
    case STARTUP:
    {
      setPoint = 0.0;

      // LCD
      lcd.setCursor(0, 0);
      lcd.print("Profile ");
      lcd.print(profile);
      lcd.print("     ");
      lcd.setCursor(11, 1);
      lcd.print("START");

      // Check button
      if (digitalRead(button) == 1)
      {
        delay(1000);
        if (digitalRead(button) == 1) // long press: start reflow
        {
          // LCD
          lcd.setCursor(11, 1);
          lcd.print(" STOP");
          lcd.setCursor(0, 0);
          lcd.print("Starting reflow");

          reflowState = HEATING;
          timeElapsed = 0;
          currentState = 0;

          delay(1000);

          Serial.println("Starting reflow");
        }
        else // short press: switch profile
        {
          profile++;
          if (profile > sizeof(Temps_profile) / sizeof(Temps_profile[0]) - 1)
          {
            profile = 0;
          }
          // LCD
          lcd.setCursor(0, 0);
          lcd.print("Profile ");
          lcd.print(profile);
          lcd.print("     ");

          Serial.print("Switched to profile: ");
          Serial.println(profile);
        }
      }
    }
    break;
    }
  }

  if (!useSimulation) // pull the input in from the real world
  {
    measuredTemp = readThermistor();
    PIDInput = measuredTemp;
  }

  if (tuning) // ********** Auto Tune Mode **********
  {
    byte val = (aTune.Runtime());
    if (val != 0)
    {
      tuning = false;
    }

    if (!tuning) // we're done, set the tuning parameters
    {
      kp = aTune.GetKp();
      ki = aTune.GetKi();
      kd = aTune.GetKd();
      myPID.SetTunings(kp, ki, kd);
      AutoTuneHelper(false);
    }
  }
  else // ********** Normal Mode **********
  {
    /*
          if (gap < aggressiveGap) // we're close to set point, use conservative tuning parameters
        {
          myPID.SetTunings(consKp, consKi, consKd);
        }
        else // we're far from set point, use aggressive tuning parameters
        {
          myPID.SetTunings(aggKp, aggKi, aggKd);
        }
        */
    myPID.Compute();
  }

  if (useSimulation)
  {
    theta[30] = PIDOutput;
    if (now >= modelTime)
    {
      modelTime += 100;
      DoModel();
    }
  }
  else
  {
    analogWrite(SSRPin, PIDOutput);
  }

  if (tuning)
  {
    if (millis() > serialTime) // send-receive with processing if it's time
    {
      SerialReceive();
      SerialSend();
      serialTime += 500;
    }
  }

  // LCD
  lcd.setCursor(0, 1);
  if (round(measuredTemp) < 100)
  {
    lcd.print(" ");
    lcd.setCursor(1, 1);
  }
  lcd.print(round(measuredTemp));

  lcd.setCursor(5, 1);
  if (round(setPoint) < 10)
  {
    lcd.print("  ");
    lcd.setCursor(7, 1);
  }
  else if (round(setPoint < 100))
  {
    lcd.print(" ");
    lcd.setCursor(6, 1);
  }
  lcd.print(round(setPoint));

  gap = abs(setPoint - PIDInput); // distance away from set point

  if (!tuning)
  {
    // PRINT
    Serial.print(timeElapsed);
    Serial.print(", ");
    Serial.print(setPoint);
    Serial.print(", ");
    Serial.print(measuredTemp);
    Serial.print(", ");
    Serial.print(PIDOutput);
    Serial.print(", ");
    Serial.println(gap);

    // ADVANCE CLOCK
    if (currentMillis - startMillis >= 1000)
    {
      timeElapsed++;
      startMillis = currentMillis;
    }
  }
}
