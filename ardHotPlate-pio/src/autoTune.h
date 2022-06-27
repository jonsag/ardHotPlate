void AutoTuneHelper(boolean start)
{
    if (start)
        ATuneModeRemember = myPID.GetMode();
    else
        myPID.SetMode(ATuneModeRemember);
}

void changeAutoTune()
{
    if (!tuning)
    {
        // Set the PIDOutput to the desired starting frequency.
        PIDOutput = aTuneStartValue;
        aTune.SetNoiseBand(aTuneNoise);
        aTune.SetOutputStep(aTuneStep);
        aTune.SetLookbackSec((int)aTuneLookBack);
        AutoTuneHelper(true);
        tuning = true;
    }
    else
    { // cancel autotune
        aTune.Cancel();
        tuning = false;
        AutoTuneHelper(false);
    }
}

void SerialSend()
{
    Serial.print("setPoint: ");
    Serial.print(setPoint);
    Serial.print(" ");
    Serial.print("PIDInput: ");
    Serial.print(PIDInput);
    Serial.print(" ");
    Serial.print("output: ");
    Serial.print(PIDOutput);
    Serial.print(" ");
    if (tuning)
    {
        Serial.println("tuning mode");
    }
    else
    {
        Serial.print("kp: ");
        Serial.print(myPID.GetKp());
        Serial.print(" ");
        Serial.print("ki: ");
        Serial.print(myPID.GetKi());
        Serial.print(" ");
        Serial.print("kd: ");
        Serial.print(myPID.GetKd());
        Serial.println();
    }
}

void SerialReceive()
{
    if (Serial.available())
    {
        char b = Serial.read();
        Serial.flush();
        if ((b == '1' && !tuning) || (b != '1' && tuning))
            changeAutoTune();
    }
}

void DoModel()
{
    // cycle the dead time
    for (byte i = 0; i < 49; i++)
    {
        theta[i] = theta[i + 1];
    }
    // compute the PIDInput
    PIDInput = (kpmodel / taup) * (theta[0] - outputStart) + PIDInput * (1 - 1 / taup) + ((float)random(-10, 10)) / 100;
}