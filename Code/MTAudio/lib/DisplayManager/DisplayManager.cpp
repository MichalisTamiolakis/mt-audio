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
    lcd->print("      MT Audio     ");
    lcd->print("      Welcome      ");

    Serial.println("[Display]: Turn on screen");
}

void DisplayManager::displayVolume()
{
    this->clearLine(1);
    this->clearLine(2);
    this->clearLine(3);

    lcd->setCursor(0,1);
    lcd->print("       Volume        ");

    Serial.println("[Display]: Volume screen");

}

void DisplayManager::updateVolume(uint8_t volume)
{
    lcd->setCursor(0,2);
    lcd->printf("         %2d         ", volume);
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

    lcd->setCursor(0,2);
    switch (source)
    {
        case AudioSource::FM1:
            lcd->print("        FM1        ");
            Serial.println("[Display]: Idle FM1 screen");
            break;
        case AudioSource::FM2:
            lcd->print("        FM2        ");
            Serial.println("[Display]: Idle FM2 screen");
            break;
        case AudioSource::FMBst:
            lcd->print("       FMBst       ");
            Serial.println("[Display]: Idle FMBst screen");
            break;
        case AudioSource::Bluetooth:
            lcd->print("     Bluetooth     ");
            break;
        case AudioSource::USB:
            lcd->print("        USB        ");
            break;
        case AudioSource::SD:
            lcd->print("         SD        ");
            break;
    }
    // lcd->setCursor(1,0);
    // lcd->print("        FM         ");
    // lcd->setCursor(2,0);
    // lcd->printf("      %2.2f MHz     ", FMFrequency);
    // lcd->setCursor(3,0);
    // lcd->printf("        %d          ", FMStationNumber);
}

void DisplayManager::displayBluetooth()
{
}

void DisplayManager::displayUSB()
{
}

void DisplayManager::displaySD()
{
}

void DisplayManager::displayAux()
{
}

void DisplayManager::clearLine(int lineNo)
{
    lcd->setCursor(0, lineNo);
    lcd->print("                   ");
}
