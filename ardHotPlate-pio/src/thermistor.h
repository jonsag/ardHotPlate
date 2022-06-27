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

    // convert the value to resistance
    average = 1023 / average - 1;
    average = seriesResistor / average;

    steinhart = average / thermistorNominal;          // (R/Ro)
    steinhart = log(steinhart);                       // ln(R/Ro)
    steinhart /= bCoefficient;                        // 1/B * ln(R/Ro)
    steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // convert absolute temp to C

    // delay(1000);
    return (steinhart);
}