#ifndef DisplayManager_h
#define DisplayManager_h

// #include <System.h>
#include <LiquidCrystal_I2C.h>

// display functions print static info
// update functions print dynamic info along with the static printed before with display functions

class DisplayManager{
public:
    DisplayManager(uint8_t lcdAddress, uint8_t backlight)
    {
        this->backlightPin = backlight;
        this->lcdAddress = lcdAddress;

        pinMode(backlight, OUTPUT);
        
        this->lcd = new LiquidCrystal_I2C(lcdAddress, 20, 4);
        lcd->init();

        powerOff();
    }

    void powerOn()
    {
        analogWrite(backlightPin, 255);
        lcd->display();
        lcd->backlight();
        isPoweredOn = true;
    }

    void powerOff()
    {
        this->isPoweredOn = false;

        analogWrite(backlightPin, 0);

        lcd->clear();
        lcd->noDisplay();
        lcd->noBacklight();
    }

    void updateTimeDisplay()
    {
        
    }

    void updateTemperatureDisplay()
    {

    }

    void updateMainDisplay()
    {

    }

private:
    bool isPoweredOn;
    uint8_t backlightPin;
    uint8_t lcdAddress;    
    LiquidCrystal_I2C* lcd;
    void clearLine(int lineNo)
    {
        lcd->setCursor(0, lineNo);
        lcd->print("                   ");
    }
};

#endif