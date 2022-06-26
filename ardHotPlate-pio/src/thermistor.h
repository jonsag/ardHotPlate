float readThermistor()
{
    for (i = 0; i < numSamples; i++) // take n samples in a row, with a slight delay
    {
        samples[i] = analogRead(thermistorPin);
        delay(readDelay);
    }

    average = 0;
    for (i = 0; i < numSamples; i++) // average all the samples out
    {
        average += samples[i];
    }
    average /= numSamples;

    if (!plot)
    {
        Serial.print("Average analog reading: ");
        Serial.print(average);
    }
        else
    {
        Serial.print(average);
        Serial.print(", ");
    }

    // convert the value to resistance
    average = 1023 / average - 1;
    average = seriesResistor / average;

    if (!plot)
    {
        Serial.print("\tThermistor resistance: ");
        Serial.print(average);
    }
    else
    {
        Serial.print(average);
        Serial.print(", ");
    }
    
    steinhart = average / thermistorNominal;          // (R/Ro)
    steinhart = log(steinhart);                       // ln(R/Ro)
    steinhart /= bCoefficient;                        // 1/B * ln(R/Ro)
    steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // convert absolute temp to C

    if (!plot)
    {
        Serial.print("\tTemperature: ");
        Serial.print(steinhart);
        Serial.print(" *C");
    }
    else
    {
        Serial.print(steinhart);
        Serial.print(", ");
    }

    lcd.setCursor(0, 0);
    lcd.print(average);

    lcd.setCursor(0, 1);
    lcd.print(steinhart);

    // delay(1000);
    return (steinhart);
}