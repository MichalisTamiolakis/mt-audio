#include "DisplayManager.h"

DisplayManager::DisplayManager(uint8_t lcdAddress, uint8_t backlight)
{
    this->backlightPin = backlight;
    this->lcdAddress = lcdAddress;

    pinMode(backlight, OUTPUT);
    
    this->lcd = new LiquidCrystal_I2C(lcdAddress, 20, 4);
    lcd->init();

    powerOff();
}

void DisplayManager::powerOn()
{
    analogWrite(backlightPin, 255);
    lcd->display();
    lcd->backlight();
    isPoweredOn = true;
    Serial.println("[Display]: Power on");
}

void DisplayManager::powerOff()
{
    this->isPoweredOn = false;

    analogWrite(backlightPin, 0);

    lcd->clear();
    lcd->noDisplay();
    lcd->noBacklight();
    Serial.println("[Display]: Power off");
}

void DisplayManager::updateTime(int newTime)
{
    lcd->setCursor(0, 0);
    lcd->print("12:00");

    Serial.println("[Display]: Update time");
}

void DisplayManager::updateTemperature(float newTemeperature)
{
    lcd->setCursor(16, 0);
    lcd->print("2.5C");
    Serial.println("[Display]: Update temperature");
}

void DisplayManager::displayTurnOn()
{
    lcd->clear();

    lcd->setCursor(0,1);
    lcd->print("       MT Audio     ");
    lcd->print("       Welcome      ");

    Serial.println("[Display]: Turn on screen");
}

void DisplayManager::displayVolume()
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);

    lcd->setCursor(0,1);
    lcd->print("       VOLUME        ");

    Serial.println("[Display]: Volume screen");

}

void DisplayManager::updateVolume(uint8_t volume)
{
    lcd->setCursor(0,2);
    lcd->printf("         %02d         ", volume);
    Serial.println("[Display]: Update volume");
}

void DisplayManager::displayBass()
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);

    lcd->setCursor(0,1);
    lcd->print("        Bass         ");
    Serial.println("[Display]: Volume screen");
}

void DisplayManager::updateBass(uint8_t bassLevel)
{
    lcd->setCursor(0,2);
    lcd->printf("         %d           ", bassLevel);
}

void DisplayManager::displayTreble()
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);

    lcd->setCursor(0,1);
    lcd->print("       Treble        ");
    Serial.println("[Display]: Volume screen");
}

void DisplayManager::updateTreble(uint8_t trebleLevel)
{
    lcd->setCursor(0,2);
    lcd->printf("         %d           ", trebleLevel);
}

void DisplayManager::displayIdle(AudioSource source)
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);

    lcd->setCursor(0,1);
    switch (source)
    {
        case AudioSource::Aux:
            lcd->print("        AUX        ");
            break;
        case AudioSource::Bluetooth:
            lcd->print("     BLUETOOTH     ");
            break;
        case AudioSource::USB:
            lcd->print("        USB        ");
            break;
        case AudioSource::SD:
            lcd->print("      SD CARD      ");
            break;
    }
    // lcd->setCursor(1,0);
    // lcd->print("        FM         ");
    // lcd->setCursor(2,0);
    // lcd->printf("      %2.2f MHz     ", FMFrequency);
    // lcd->setCursor(3,0);
    // lcd->printf("        %d          ", FMStationNumber);
}

void DisplayManager::displayStandbyIdle()
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);
}

void DisplayManager::updateFM(uint16_t frequency, FMBand savedBand, const char* serviceName, const char* radioText)
{
    clearLine(1);
    float freqFloat = (float)frequency / 100.0f;
    lcd->setCursor(0,1);    
    switch(savedBand)
    {
        case FMBand::FM:
            lcd->print("      ");
            break;
        case FMBand::FM1:
            lcd->print("    FM1 ");
            break;
        case FMBand::FM2:
            lcd->print("    FM2 ");
            break;
        case FMBand::FMBst:
            lcd->print("    BST ");
            break;
    }
    lcd->printf("%2.2f MHz", freqFloat);

    if(serviceName != nullptr)
    {
        clearLine(2);
        lcd->setCursor(0,2);
        lcd->printf("      %s      ", serviceName);
    }

    if(radioText != nullptr)
    {
        clearLine(3);
        // TODO: Add scrolling text for radio text
        // lcd->setCursor(0,3);
        // lcd->printf("   %s", radioText);
    }
}

void DisplayManager::displayInputSelection()
{
    lcd->setCursor(0,1);
    lcd->print("       INPUT:        ");
    this->clearLine(2);
    this->clearLine(3);
}

void DisplayManager::updateInputSelection(AudioSource source, FMBand fmBand)
{

    lcd->setCursor(0,2);
    switch(source){
        case AudioSource::Radio:
            switch (fmBand)
            {
                case FMBand::FM:
                case FMBand::FM1:
                    lcd->print("      RADIO FM1     ");
                    break;
                case FMBand::FM2:
                    lcd->print("      RADIO FM2     ");
                    break;
                case FMBand::FMBst:
                    lcd->print("BEST SIGNAL STATIONS");
                    break;
            }
            break;
        case AudioSource::Aux:
            lcd->print("        AUX         ");
            break;
        case AudioSource::Bluetooth:
            lcd->print("     BLUETOOTH      ");
            break;
        case AudioSource::USB:
            lcd->print("        USB         ");
            break;
        case AudioSource::SD:
            lcd->print("      SD CARD       ");
            break;
      }
}

void DisplayManager::displayStationSave()
{
    lcd->setCursor(0,1);
    lcd->printf("  STATION SAVED IN: ");
    clearLine(2);
    clearLine(3);
}

void DisplayManager::updateStationSave(uint16_t frequency, FMBand fmBand)
{
    lcd->setCursor(0,2);
    switch(fmBand)
    {
        case FMBand::FM1:
            lcd->print("        FM1         ");
            break;
        case FMBand::FM2:
            lcd->print("        FM2         ");
            break;
        case FMBand::FMBst:
            lcd->print("        BST         ");
            break;
    }
}

void DisplayManager::clearLine(int lineNo)
{
    lcd->setCursor(0, lineNo);
    lcd->print("                   ");
}
