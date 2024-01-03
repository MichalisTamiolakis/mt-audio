#ifndef DisplayManager_h
#define DisplayManager_h

// #include <System.h>
#include <LiquidCrystal_I2C.h>
#include <stdbool.h>

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

        lcd->createChar(0, celsiusIcon);

        powerOff();
    }

    void powerOn()
    {
        analogWrite(backlightPin, brightness);
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

    void setBrightness(uint8_t value)
    {
        this->brightness = value;
        if(this->isPoweredOn)
        {
            analogWrite(backlightPin, this->brightness);
        }
    }

    void updateTimeDisplay(uint8_t hour, uint8_t minute, uint8_t second)
    {
        // char time_display[5];
        // sprintf(time_display, "%02d:%02d", hour, minute);
        lcd->setCursor(15,0);
        if(second % 2 == 0)
            lcd->printf("%02d %02d", hour, minute);
        else
            lcd->printf("%02d:%02d", hour, minute);
    }

    void updateTemperatureDisplay(float temperature)
    {

    }

    // Main Displays
    void standbyDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, float temperatureIn, float temperatureOut)
    {
        lcd->setCursor(0,0);
        lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        lcd->setCursor(0,1);
        lcd->printf("      %02d:%02d:%02d      ", hour, minute, second);
        lcd->setCursor(0,2);
        lcd->printf("       Tin: %02.1f", temperatureIn);
        lcd->setCursor(0,3);
        lcd->printf("       Tout: %02.1f", temperatureOut);
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
        lcd->print("      MT Audio      ");
        lcd->setCursor(0, 2);
        lcd->print("     v1.1.0  by     ");
        lcd->setCursor(0, 3);
        lcd->print("Michalis Tamiolakis ");
    }

    void fullDateTimeDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t dayOfWeek, uint8_t hour, uint8_t minute, uint8_t second)
    {
        this->clearLine(0);
        lcd->setCursor(0, 1);
        lcd->printf("%s", this->dayOfWeek[dayOfWeek]);
        lcd->setCursor(0, 2);
        lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        lcd->setCursor(0, 3);
        lcd->printf("      %02d:%02d:%02d      ", hour, minute, second);
    }

    /// @brief Display the date set screen
    /// @param date 
    /// @param month 
    /// @param year 
    /// @param hour 
    /// @param minute 
    /// @param second 
    /// @param charsOff Whether the date chars should be turned off or not (used for blinking to show the user which char is being changed)
    void dateSetDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, bool charsOff = false)
    {
        clearLine(0);
        lcd->setCursor(0,1);
        if(charsOff)
        {
            lcd->printf("       .%02d.%04d     ", month, year);
        }
        else
        {
            lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        }
        lcd->setCursor(0,2);
        if(second%2 == 0)
        {
            lcd->printf("       %02d:%02d        ", hour, minute);
        }
        else
        {
            lcd->printf("       %02d %02d        ", hour, minute);
        }
        clearLine(3);
    }

    /// @brief Display the month set screen
    /// @param date 
    /// @param month 
    /// @param year 
    /// @param hour 
    /// @param minute 
    /// @param second 
    /// @param charsOff Whether the date chars should be turned off or not (used for blinking to show the user which char is being changed)
    void monthSetDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, bool charsOff = false)
    {
        clearLine(0);
        lcd->setCursor(0,1);
        if(charsOff)
        {
            lcd->printf("     %02d.  .%04d     ", date, year);
        }
        else
        {
            lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        }
        lcd->setCursor(0,2);
        if(second%2 == 0)
        {
            lcd->printf("       %02d:%02d        ", hour, minute);
        }
        else
        {
            lcd->printf("       %02d %02d        ", hour, minute);
        }
        clearLine(3);
    }

    /// @brief Display the year set screen
    /// @param date 
    /// @param month 
    /// @param year 
    /// @param hour 
    /// @param minute 
    /// @param second 
    /// @param charsOff Whether the date chars should be turned off or not (used for blinking to show the user which char is being changed)
    void yearSetDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, bool charsOff = false)
    {
        clearLine(0);
        lcd->setCursor(0,1);
        if(charsOff)
        {
            lcd->printf("     %02d.%02d.         ", date, month);
        }
        else
        {
            lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        }
        lcd->setCursor(0,2);
        if(second%2 == 0)
        {
            lcd->printf("       %02d:%02d        ", hour, minute);
        }
        else
        {
            lcd->printf("       %02d %02d        ", hour, minute);
        }
        clearLine(3);
    }

    /// @brief Display the hour set screen
    /// @param date 
    /// @param month 
    /// @param year 
    /// @param hour 
    /// @param minute 
    /// @param second 
    /// @param charsOff Whether the date chars should be turned off or not (used for blinking to show the user which char is being changed)
    void hourSetDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, bool charsOff = false)
    {
        clearLine(0);
        lcd->setCursor(0,1);
        lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        lcd->setCursor(0,2);
        if (charsOff)
        {
            lcd->printf("         :%02d        ", minute);
        }
        else
        {
            lcd->printf("       %02d:%02d        ", hour, minute);
        }
        clearLine(3);
    }

    /// @brief Display the minute set screen
    /// @param date 
    /// @param month 
    /// @param year 
    /// @param hour 
    /// @param minute 
    /// @param second 
    /// @param charsOff Whether the date chars should be turned off or not (used for blinking to show the user which char is being changed)
    void minuteSetDisplay(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, bool charsOff = false)
    {
        clearLine(0);
        lcd->setCursor(0,1);
        lcd->printf("     %02d.%02d.%04d     ", date, month, year);
        lcd->setCursor(0,2);
        if (charsOff)
        {
            lcd->printf("       %02d:          ", hour);
        }
        else
        {
            lcd->printf("       %02d:%02d        ", hour, minute);
        }
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

    void balanceDisplay(int8_t balance)
    {
        uint8_t left = balance < 0 ? abs(balance) : 0;
        uint8_t right = balance > 0 ? balance : 0;

        lcd->setCursor(0,1);
        lcd->print("       BALANCE      ");
        lcd->setCursor(0,2);
        lcd->printf("        %d  %d        ", left, right);
        clearLine(3);
    }

    void fadeDisplay(int8_t fade)
    {
        uint8_t front = fade < 0 ? abs(fade) : 0;
        uint8_t rear = fade > 0 ? fade : 0;

        lcd->setCursor(0,1);
        lcd->print("        FADE        ");
        lcd->setCursor(0,2);
        lcd->printf("        %d           ", front);
        lcd->setCursor(0,3);
        lcd->printf("        %d           ", rear);
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

    void autoStoreSearchInProgressDisplay()
    {
        lcd->setCursor(0,1);
        lcd->print("BEST SIGNAL STATIONS");
        lcd->setCursor(0,2);
        lcd->print("    SEARCHING...    ");
        clearLine(3);
    }

private:
    bool isPoweredOn;
    uint8_t brightness = 255;
    uint8_t backlightPin;
    uint8_t lcdAddress;    
    LiquidCrystal_I2C* lcd;

    // Day of week
    char dayOfWeek[7][21] = {
        "       Sunday       ",
        "       Monday       ",
        "       Tuesday      ",
        "      Wednesday     ",
        "      Thursday      ",
        "       Friday       ",
        "      Saturday      "
    };

    // Custom Characters
    byte celsiusIcon[8] = {
        B11000,
        B11000,
        B00111,
        B01000,
        B01000,
        B01000,
        B01000,
        B00111
    };

    void clearLine(int lineNo)
    {
        lcd->setCursor(0, lineNo);
        lcd->print("                    ");
    }
};

#endif