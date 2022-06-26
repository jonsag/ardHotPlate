#include <Arduino.h>

int ThermistorPin = 0;
int Vo;
float R1 = 100000;
float logR2, R2, Tf, Tc;
float c1 = 1.009249522e-03;
float c2 = 2.378405444e-04;
float c3 = 2.019202697e-07;

void setup()
{
  Serial.begin(9600);
}

void loop()
{

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  Tf = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tf = Tf - 273.15;
  Tf = (Tf * 9.0) / 5.0 + 32.0;

  Tf = Tf + 80;
  
  Tc = (Tf - 32) / 1.8;

  Serial.print("Raw: ");
  Serial.print(Vo);
  Serial.print("\tTemperature: ");
  Serial.print(Tf);
  Serial.print(" F\t");
  Serial.print(Tc * 1.35);
  Serial.println(" C");

  delay(500);
}