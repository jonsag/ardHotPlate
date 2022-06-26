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
