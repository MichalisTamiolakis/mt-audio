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

    void updateTimeDisplay(float time)
    {
        
    }

    void updateTemperatureDisplay(float temperature)
    {

    }

    // Main Displays
    void standbyDisplay(uint8_t date, uint8_t month, uint16_t year, uint8_t hours, uint8_t minutes, float temperatureIn, float temperatureOut)
    {
        char time_display[5];
        char date_display[10];

        lcd->clear();
        lcd->setCursor(0,0);
        sprintf(date_display, "%02d-%02d-20%02d", date, month, year);
        lcd->print("     "); lcd->print(date_display);
        sprintf(time_display, "%02d:%02d", hours, minutes);
        lcd->setCursor(0,1);
        lcd->print("       "); lcd->print(time_display);
        lcd->setCursor(0,2);
        lcd->print("       Tin: ");
        lcd->print(temperatureIn, 1);
        lcd->setCursor(0,3);
        lcd->print("       Tout:--      ");
    }

    void fmDisplay(FMBand band, uint16_t frequency, char* stationName)
    {
        clearLine(1);
        lcd->setCursor(0,1);
        lcd->print("   "); 
        switch(band)
        {
            case FMBand::FM1:
                lcd->print("FM1 ");
                break;
            case FMBand::FM2:
                lcd->print("FM2 ");
                break;
            case FMBand::FMBst:
                lcd->print("BST ");
                break;
            default:
                lcd->print("    ");
                break;
        }

        lcd->print((float)frequency/100); 
        lcd->print(" MHz");

        if(stationName != nullptr)
        {
            lcd->setCursor(0,2);
            lcd->print("      "); lcd->print(stationName); lcd->print("      ");
        }
        else
        {
            clearLine(2);
        }
        clearLine(3);
    }

    void auxDisplay()
    {
        clearLine(1);
        lcd->setCursor(0,2);
        lcd->print("        AUX         ");
        clearLine(3);
    }

    void btDisplay()
    {
        clearLine(1);
        lcd->setCursor(0,2);
        lcd->print("     BLUETOOTH      ");
        clearLine(3);
    }

    void usbDisplay()
    {
        clearLine(1);
        lcd->setCursor(0,2);
        lcd->print("        USB         ");
        clearLine(3);
    }

    void sdDisplay()
    {
        clearLine(1);
        lcd->setCursor(0,2);
        lcd->print("          SD         ");
        clearLine(3);
    }

    // --- Temporary Displays ---
    void welcomeDisplay()
    {
        clearLine(0);
        lcd->setCursor(0,1);
        lcd->print(" MT Audio");
        lcd->setCursor(0, 2);
        lcd->print("v1.0");
        clearLine(3);
    }

    void sourceChangeDisplay(AudioSource source, FMBand band)
    {
        lcd->setCursor(0,1);
        lcd->print("       INPUT:       ");
        lcd->setCursor(0,2);
        switch(source){
            case AudioSource::Radio:
                switch(band)
                {
                    case FMBand::FM1:
                        lcd->print("      RADIO FM1     ");
                        break;
                    case FMBand::FM2:
                        lcd->print("      RADIO FM2     ");
                        break;
                    case FMBand::FMBst:
                        lcd->print("BEST SIGNAL STATIONS");
                        break;
                    default:
                        lcd->print("      RADIO FM      ");
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
                lcd->print("         SD         ");
                break;
        }
        clearLine(3);
    }

    void volumeDisplay(uint8_t volume)
    {
        lcd->setCursor(0,1);
        lcd->print("       VOLUME       ");
        lcd->setCursor(0,2);
        lcd->print("         ");
        lcd->print(volume);
        lcd->print("         ");
        clearLine(3);
    }

    void bassDisplay(uint8_t bass)
    {
        lcd->setCursor(0,1);
        lcd->print("        BASS        ");
        lcd->setCursor(0,2);
        lcd->print("         ");
        lcd->print(bass - 7);
        lcd->print("         ");
        clearLine(3);
    }

    void trebleDisplay(uint8_t treble)
    {
        lcd->setCursor(0,1);
        lcd->print("       TREBLE       ");
        lcd->setCursor(0,2);
        lcd->print("         ");
        lcd->print(treble - 7);
        lcd->print("         ");
        clearLine(3);
    }

    void balanceDisplay(uint8_t balance)
    {
        lcd->setCursor(0,1);
        lcd->print("       BALANCE      ");
        lcd->setCursor(0,2);
        lcd->print("         ");
        lcd->print(balance);
        lcd->print("         ");
        clearLine(3);
    }

    void faderDisplay(uint8_t fader)
    {
        lcd->setCursor(0,1);
        lcd->print("        FADER       ");
        lcd->setCursor(0,2);
        lcd->print("         ");
        lcd->print(fader);
        lcd->print("         ");
        clearLine(3);
    }

    void loudnessDisplay(bool loudness)
    {
        lcd->clear();
        lcd->setCursor(0,1);
        lcd->print("      LOUDNESS      ");
        lcd->setCursor(0,2);
        if(loudness)
            lcd->print("        ON        ");
        else
            lcd->print("        OFF       ");
    }

    // Radio Only Displays
    void seekModeDisplay(RadioSeekMode mode)
    {
        lcd->setCursor(0,1);
        lcd->print("    SEEK MODE:      ");
        lcd->setCursor(0,2);
        switch(mode)
        {
            case RadioSeekMode::Auto:
                lcd->print("        AUTO        ");
                break;
            case RadioSeekMode::Manual:
                lcd->print("       MANUAL       ");
                break;
        }
        clearLine(3);
    }
    
    void saveStationDisplay(FMBand savedIn)
    {
        lcd->setCursor(0,1);
        lcd->print("  STATION SAVED IN  ");
        lcd->setCursor(0,2);
        switch(savedIn)
        {
            case FMBand::FM1:
                lcd->print("        FM1         ");
                break;
            case FMBand::FM2:
                lcd->print("        FM2         ");
                break;
            case FMBand::FMBst:
                lcd->print("     RADIO BEST     ");
                break;
            default:
                lcd->print("       ERROR        ");
                break;
        }
        clearLine(3);
    }

private:
    bool isPoweredOn;
    uint8_t backlightPin;
    uint8_t lcdAddress;    
    LiquidCrystal_I2C* lcd;
    void clearLine(int lineNo)
    {
        lcd->setCursor(0, lineNo);
        lcd->print("                    ");
    }
};

#endif