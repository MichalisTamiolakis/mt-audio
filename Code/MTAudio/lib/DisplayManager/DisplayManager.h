#ifndef DisplayManager_h
#define DisplayManager_h

#include <System.h>
#include <FSM.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager{
public:
    DisplayManager(uint8_t lcdAddress, uint8_t backlight);

    void powerOn();
    void powerOff();
    void updateTime(int newTime);
    void updateTemperature(float newTemeperature);
    
    // Overlayed displays
    void displayTurnOn();

    void displayVolume();
    void updateVolume(uint8_t volumeLevel);

    void displayBass();
    void updateBass(uint8_t bassLevel);
    
    void displayTreble();
    void updateTreble(uint8_t trebleLevel);
    
    // Continuous displays
    void displayIdle(AudioSource source); // Idle for specific source
    
    // For source fm
    void updateFMFrequency();


    void displayBluetooth();
    void displayUSB();
    void displaySD();
    void displayAux();

private:
    bool isPoweredOn;
    uint8_t backlightPin;
    uint8_t lcdAddress;    
    LiquidCrystal_I2C* lcd;
    char* displayChars; 
    void clearLine(int lineNo);

};

#endif