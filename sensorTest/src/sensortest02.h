#include <Arduino.h>

// LCD

// include the library code:
#include <LiquidCrystal595.h>

// initialize the library with the numbers of the interface pins + the row count
// datapin, latchpin, clockpin, num_lines
#define Data 2
#define Latch 3
#define Clock 4
LiquidCrystal595 lcd(Data, Latch, Clock);

// SPDX-FileCopyrightText: 2011 Limor Fried/ladyada for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Thermistor Example #3 from the Adafruit Learning System guide on Thermistors
// https://learn.adafruit.com/thermistor/overview by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

// which analog pin to connect
#define THERMISTORPIN A0

// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000

// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25

// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5

// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

// the value of the 'other' resistor
#define SERIESRESISTOR 100000

int samples[NUMSAMPLES];

void backlightOn(boolean on)
{
    if (on)
    {
        lcd.setLED2Pin(HIGH);
        lcd.display();
    }
    else
    {
        lcd.setLED2Pin(LOW);
        lcd.noDisplay();
    }
}

void setup(void)
{
    Serial.begin(9600);
    // analogReference(EXTERNAL);

    lcd.begin(16, 2);

    backlightOn(HIGH);
}

void loop(void)
{
    uint8_t i;
    float average;

    // take N samples in a row, with a slight delay
    for (i = 0; i < NUMSAMPLES; i++)
    {
        samples[i] = analogRead(THERMISTORPIN);
        delay(10);
    }

    // average all the samples out
    average = 0;
    for (i = 0; i < NUMSAMPLES; i++)
    {
        average += samples[i];
    }
    average /= NUMSAMPLES;

    Serial.print("Average analog reading ");
    Serial.print(average);

    // convert the value to resistance
    average = 1023 / average - 1;
    average = SERIESRESISTOR / average;
    Serial.print("\tThermistor resistance ");
    Serial.print(average);

    float steinhart;
    steinhart = average / THERMISTORNOMINAL;          // (R/Ro)
    steinhart = log(steinhart);                       // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                        // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // convert absolute temp to C

    Serial.print("\tTemperature ");
    Serial.print(steinhart);
    Serial.println(" *C");

    lcd.setCursor(0, 0);
    lcd.print(average);

    lcd.setCursor(0, 1);
    lcd.print(steinhart);

    delay(1000);
}
