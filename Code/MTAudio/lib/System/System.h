#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <string.h>
#include <Enums.h>
#include <DisplayManager.h>

// Delay helper
#include <AsyncDelayHelper.h>

// Buttons
#include <SingleButton.h>
#include <MultiplexedButton.h>

// TDA7313
#include <Tda7313.h>

// BT201
#include <BT201.h>

// SI4703
#include <radio.h>
#include <SI4703.h>
#include <RDSParser.h>

// DS3231
#include <RTClib.h>

#define DEBUG_LOG_ENABLED

#ifdef DEBUG_LOG_ENABLED
#define DEBUG_LOG(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define DEBUG_LOG(format, ...)
#endif

#define RADIO_ENABLED

// TDA ADDRESS
#define TDA_ADDRESS 0x44
#define LCD_ADDRESS 0x3F

// Serial 2 pins
#define RXD2 16
#define TXD2 17

// Output
#define PWR_ENABLE 15
#define BTN_BACKLIGHT 4
#define SCREEN_BACKLIGHT 2
#define SDA_PIN 21
#define SCL_PIN 22
#define RADIO_RST 19

// Input
#define S_LIGHT 35
#define S_TEMPERATURE 34

// Audio Sources
#define AUDIO_IN_RADIO 2
#define AUDIO_IN_BT_USB_SD 3
#define AUDIO_IN_AUX 1

// Generic Settings
#define SAVE_STATION_PAUSE 300 // 300 ms pause when saving station

// Overlay Menu display times
#define TURN_ON_SEQUENCE_DISPLAY_TIME 3000
#define VOLUME_DISPLAY_TIME 2000
#define SAVE_STATION_DISPLAY_TIME 2000
#define INPUT_SELECTION_DISPLAY_TIME 2000
#define BASS_DISPLAY_TIME 2000
#define BALANCE_DISPLAY_TIME 2000
#define TREBLE_DISPLAY_TIME 2000
#define FADER_DISPLAY_TIME 2000
#define LOUDNESS_DISPLAY_TIME 2000
#define SEEK_MODE_DISPLAY_TIME 2000
#define FULL_DATETIME_DISPLAY_TIME 5000
#define DATETIME_SET_DISPLAY_TIME 10000

class System
{
private:
    // Display
    DisplayManager *display;

    // Radio
    SI4703 radio = SI4703();
// static RDSParser *rdsParser;
#ifdef RADIO_ENABLED
// static String rdsServiceName;
// static String rdsRadioText;
#endif
    uint16_t stationAtShutdown = 8870;

    // TDA7313
    Tda7313 *tda;

    // BT201
    BT201 *bt201;

    // RTC
    RTC_DS3231 *rtc;

    SystemState stateBeforeIgnitionOn = SystemState::Off;
    SystemState stateBeforeIgnitionOff = SystemState::Standby;

    AsyncDelayHelper *delayHelper;
    bool hasDelayStarted = false;
    SystemMode delayMode; // Mode to switch to after delay

    // Async Updates for Temp/Time/Brightness
    AsyncDelayHelper *timeUpdateDelay;

    bool ignitionState;

    // Radio specific data
    uint16_t savedStations[3][6] = {
        {8870, 8870, 8870, 8870, 8870, 8870},
        {8870, 8870, 8870, 8870, 8870, 8870},
        {8870, 8870, 8870, 8870, 8870, 8870}};

    bool isCurrentStationSaved = false;
    FMBand currentSavedStationBand = FMBand::FM1;

    void initRadio()
    {
#ifdef RADIO_ENABLED
        radio.setup(RADIO_RESETPIN, RADIO_RST);
        radio.setup(RADIO_MODEPIN, SDA_PIN);

        // // Enable information to the Serial port
        // radio.debugEnable(true);
        // radio._wireDebug(true);

        // // Set FM Options for Europe
        radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);  // for EUROPE
        radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50); // for EUROPE

        // // Initialize the Radio
        radio.initWire(Wire);

        // radio.debugEnable(true);
        // radio._wireDebug(true);

        // // Set all radio setting to the fixed values.
        radio.setBandFrequency(RADIO_BAND_FM, stationAtShutdown);
        radio.setVolume(15);
        radio.setMono(false);
        radio.setMute(false);
        radio.setSoftMute(true);
#endif
    }

    void shutdownRadio()
    {
#ifdef RADIO_ENABLED
        stationAtShutdown = radio.getFrequency();
        radio.setMute(true);
#endif
    }

    void initTDA()
    {
        tda->input(AUDIO_IN_RADIO); // 1,2,3    Stereo 1, Stereo 2, Stereo 3
        tda->loud(false);           // Loud off (true,false)
        tda->mute(false);           // Mute off (true,false)
        tda->volume(7);             //  0.......15  -78.75dB...........0dB
        tda->bass(7);               // 0....7...14  -14dB.....0dB....+14dB
        tda->treble(7);             // 0....7...14  -14dB.....0dB....+14dB

        tda->sla(2); // 0,1,2,3  0dB, +3.75dB, +7.5dB, +11.25dB

        tda->attLF(0); //  0.......13  0dB.........-36.25dB
        tda->attRF(0); //  0.......13  0dB.........-36.25dB
        tda->attLR(0); //  0.......13  0dB.........-36.25dB
        tda->attRR(0); //  0.......13  0dB.........-36.25dB
    }

    void initRTC()
    {
        rtc->begin();
        DateTime dt(2021, 1, 1, 0, 0, 0);
        rtc->adjust(dt);
    }

    void updateSystemState(SystemState newState)
    {
        SystemState prevState = systemState;
        systemState = newState;

        // State change functionality
        switch (newState)
        {
        case SystemState::On:
            digitalWrite(PWR_ENABLE, HIGH);
            analogWrite(BTN_BACKLIGHT, 120);
            analogWrite(SCREEN_BACKLIGHT, 120);
            delay(100);
            display->powerOn();

            updateSystemMode(SystemMode::TurnOnSequence);
            delay(100);

            initRadio();
            tda->sync();

            DEBUG_LOG("System on\n");

            break;
        case SystemState::Off:
            updateSystemMode(SystemMode::Idle);

            // Shutdown the ICs here
            // Came to off from on?
            if (prevState == SystemState::On)
            {
                shutdownRadio();
            }
            display->powerOff();

            delay(100);
            digitalWrite(PWR_ENABLE, LOW);
            analogWrite(BTN_BACKLIGHT, 0);
            analogWrite(SCREEN_BACKLIGHT, 0);

            DEBUG_LOG("System off\n");

            break;
        case SystemState::Standby:

            // Came to standby from on?
            if (prevState == SystemState::On)
            {
                shutdownRadio();
            }

            delay(100);
            digitalWrite(PWR_ENABLE, LOW);
            analogWrite(BTN_BACKLIGHT, 0);
            analogWrite(SCREEN_BACKLIGHT, 120);

            display->powerOn();
            updateSystemMode(SystemMode::Idle);

            DEBUG_LOG("System Standby\n");

            break;
        }

        DEBUG_LOG("[S]: %d\n", (int)newState);
    }

    void updateSystemMode(SystemMode newMode)
    {
        SystemMode prevMode = systemMode;
        systemMode = newMode;
        DateTime now;

        // Do delayed transition stuff here.
        stopDelayedModeTransition();
        switch (newMode)
        {
        // Main Screens
        default:
        case SystemMode::Idle:
            if (systemState == SystemState::On)
            {
#ifdef RADIO_ENABLED
                uint16_t freq = radio.getFrequency();
#else
                uint16_t freq = 8870;
#endif
                switch (audioSource)
                {
                case AudioSource::Radio:
                    display->fmDisplay(isCurrentStationSaved ? band : FMBand::FM, freq, nullptr);
                    break;
                case AudioSource::Bluetooth:
                    display->btDisplay();
                    break;
                case AudioSource::Aux:
                    display->auxDisplay();
                    break;
                case AudioSource::USB:
                    display->usbDisplay();
                    break;
                case AudioSource::SD:
                    display->sdDisplay();
                    break;
                }
                updateDateTime();
            }
            else if (systemState == SystemState::Standby)
            {
                updateDateTime();
            }
            break;

        // Overlay screens
        case SystemMode::TurnOnSequence:
            startDelayedModeTransition(SystemMode::Idle, TURN_ON_SEQUENCE_DISPLAY_TIME);
            display->welcomeDisplay();
            break;
        case SystemMode::Volume:
            startDelayedModeTransition(SystemMode::Idle, VOLUME_DISPLAY_TIME);
            display->volumeDisplay(tda->volume());
            break;
        case SystemMode::StationSave:
            startDelayedModeTransition(SystemMode::Idle, SAVE_STATION_DISPLAY_TIME);
            display->saveStationDisplay(isCurrentStationSaved ? currentSavedStationBand : FMBand::FM);
            break;
        case SystemMode::InputSelection:
            startDelayedModeTransition(SystemMode::Idle, INPUT_SELECTION_DISPLAY_TIME);
            display->sourceChangeDisplay(audioSource, band);
            break;
        case SystemMode::Bass:
            startDelayedModeTransition(SystemMode::Idle, BASS_DISPLAY_TIME);
            display->bassDisplay(tda->bass());
            break;
        case SystemMode::Balance:
            startDelayedModeTransition(SystemMode::Idle, BALANCE_DISPLAY_TIME);
            display->balanceDisplay(0);
            break;
        case SystemMode::Treble:
            startDelayedModeTransition(SystemMode::Idle, TREBLE_DISPLAY_TIME);
            display->trebleDisplay(tda->treble());
            break;
        case SystemMode::Fader:
            startDelayedModeTransition(SystemMode::Idle, FADER_DISPLAY_TIME);
            display->faderDisplay(0);
            break;
        case SystemMode::Loudness:
            startDelayedModeTransition(SystemMode::Idle, LOUDNESS_DISPLAY_TIME);
            display->loudnessDisplay(tda->loud());
            break;
        case SystemMode::SeekModeSet:
            startDelayedModeTransition(SystemMode::Idle, SEEK_MODE_DISPLAY_TIME);
            display->seekModeDisplay(seekMode);
            break;
        case SystemMode::ShowFullDateTime:
            now = rtc->now();
            startDelayedModeTransition(SystemMode::Idle, FULL_DATETIME_DISPLAY_TIME);
            display->fullDateTimeDisplay(now.year(), now.month(), now.day(), now.dayOfTheWeek(), now.hour(), now.minute(), now.second());
            break;

        // Date Time Set
        case SystemMode::DateSet:
            startDelayedModeTransition(SystemMode::Idle, DATETIME_SET_DISPLAY_TIME);
            now = rtc->now();
            display->dateSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), prevMode != newMode);
            timeUpdateDelay->restartDelay();
            break;
        case SystemMode::MonthSet:
            startDelayedModeTransition(SystemMode::Idle, DATETIME_SET_DISPLAY_TIME);
            now = rtc->now();
            display->monthSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), prevMode != newMode);
            timeUpdateDelay->restartDelay();
            break;
        case SystemMode::YearSet:
            startDelayedModeTransition(SystemMode::Idle, DATETIME_SET_DISPLAY_TIME);
            now = rtc->now();
            display->yearSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), prevMode != newMode);
            timeUpdateDelay->restartDelay();
            break;
        case SystemMode::HourSet:
            startDelayedModeTransition(SystemMode::Idle, DATETIME_SET_DISPLAY_TIME);
            now = rtc->now();
            display->hourSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), prevMode != newMode);
            timeUpdateDelay->restartDelay();
            break;
        case SystemMode::MinuteSet:
            startDelayedModeTransition(SystemMode::Idle, DATETIME_SET_DISPLAY_TIME);
            now = rtc->now();
            display->minuteSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), prevMode != newMode);
            timeUpdateDelay->restartDelay();
            break;
        }

        DEBUG_LOG("[M]: %d\n", (int)newMode);
    }

    /// @brief Does a transition to the given mode after a delay.
    /// @param delayMode
    void startDelayedModeTransition(SystemMode delayMode, unsigned long delay)
    {
        this->delayMode = delayMode;
        delayHelper->startDelay(delay);
        hasDelayStarted = true;
    }

    /// @brief Stops the current delayed mode transition, if any.
    void stopDelayedModeTransition()
    {
        delayHelper->stopDelay();
        hasDelayStarted = false;
    }

    void updateDateTime()
    {
        DateTime now = rtc->now();
        switch (systemState)
        {
        case SystemState::On:
            switch(systemMode)
            {
                case SystemMode::ShowFullDateTime:
                    display->fullDateTimeDisplay(now.year(), now.month(), now.day(), now.dayOfTheWeek(), now.hour(), now.minute(), now.second());
                    break;
                case SystemMode::Idle:
                    display->updateTimeDisplay(now.hour(), now.minute(), now.second());
                    break;
            }
            break;
        case SystemState::Standby:
            switch (systemMode)
            {
            case SystemMode::Idle:
                display->standbyDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), rtc->getTemperature(), 25.0f);
                break;
            case SystemMode::DateSet:
                display->dateSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.second() % 2 == 0);
                break;
            case SystemMode::MonthSet:
                display->monthSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.second() % 2 == 0);
                break;
            case SystemMode::YearSet:
                display->yearSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.second() % 2 == 0);
                break;
            case SystemMode::HourSet:
                display->hourSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.second() % 2 == 0);
                break;
            case SystemMode::MinuteSet:
                display->minuteSetDisplay(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.second() % 2 == 0);
                break;
            }
            break;
        default:
            break;
        }
    }

    void storeRadioStation(uint8_t slot)
    {
#ifdef RADIO_ENABLED
        savedStations[(int)band][slot] = radio.getFrequency();
#endif
        isCurrentStationSaved = true;
        currentSavedStationBand = band;
        tda->mute(true);
        delay(SAVE_STATION_PAUSE);
        tda->mute(false);
    }

    void tuneToSavedRadioStation(uint8_t slot)
    {
#ifdef RADIO_ENABLED
        radio.setFrequency(savedStations[(int)band][slot]);
#endif
        isCurrentStationSaved = true;
        currentSavedStationBand = band;
    }

public:
    SystemState systemState = SystemState::Off;
    SystemMode systemMode = SystemMode::Idle;
    AudioSource audioSource;
    FMBand band = FMBand::FM1;
    RadioSeekMode seekMode = RadioSeekMode::Auto;

    System()
    {
        stationAtShutdown = 8870;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                savedStations[i][j] = 8870;
            }
        }

        // Delay Helper
        delayHelper = new AsyncDelayHelper();
        hasDelayStarted = false;
        delayMode = SystemMode::Idle;

        timeUpdateDelay = new AsyncDelayHelper();

        // Radio
        // this.rdsParser = rdsParser;

        // TDA7313
        tda = new Tda7313(TDA_ADDRESS);

        // BT201
        bt201 = new BT201(&Serial2);

        // DS3231
        rtc = new RTC_DS3231();

        systemState = SystemState::Off;
        systemMode = SystemMode::Idle;
        audioSource = AudioSource::Radio;
        ignitionState = false;

        stateBeforeIgnitionOn = SystemState::Off;
        stateBeforeIgnitionOff = SystemState::Standby;
    }

    /// @brief Should be called in init, to initialize the buttons, display, etc.
    void init()
    {
#ifdef DEBUG_LOG_ENABLED
        Serial.begin(115200);
#endif
        DEBUG_LOG("System initializing...\n");

        // Output
        pinMode(PWR_ENABLE, OUTPUT);
        digitalWrite(PWR_ENABLE, LOW);
        pinMode(BTN_BACKLIGHT, OUTPUT);
        analogWrite(BTN_BACKLIGHT, 0);

        // Input sensors
        pinMode(S_TEMPERATURE, INPUT_PULLDOWN);
        pinMode(S_LIGHT, INPUT_PULLDOWN);

        delay(200);
        DEBUG_LOG("System initializing... Setting up radio\n");
        initRadio();

        delay(200);
        DEBUG_LOG("System initializing... Setting up display\n");
        display = new DisplayManager(LCD_ADDRESS, SCREEN_BACKLIGHT);

        delay(200);
        DEBUG_LOG("System initializing... Setting up EQ\n");
        initTDA();

        delay(200);
        DEBUG_LOG("System initializing... Setting up RTC\n");
        initRTC();

        delay(200);
        DEBUG_LOG("System initializing... Setting Serial2 communication\n");
        Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

        // Start delay helpers
        timeUpdateDelay->startDelay(1000);

        DEBUG_LOG("System initialized\n");
    }

    /// @brief Should be called every loop to update input, display, etc.
    void update()
    {
#ifdef RADIO_ENABLED
        // radio.checkRDS();
#endif

        // Delay check
        delayHelper->loop();
        if (hasDelayStarted && delayHelper->hasDelayFinished())
        {
            hasDelayStarted = false;
            updateSystemMode(delayMode);
        }

        // Async Updates for Time/Temp/Brightness (Only while system is on or idle)
        if (systemState != SystemState::Off)
        {
            timeUpdateDelay->loop();
            if (timeUpdateDelay->hasDelayFinisedThisLoop())
            {
                updateDateTime();
                timeUpdateDelay->restartDelay();
            }
        }
    }

    // static void RDSProcess(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
    // {
    //     rdsParser->processData(block1, block2, block3, block4);
    // }

    // static void RDSServiceNameUpdate(const char *serviceName)
    // {
    //     rdsServiceName = serviceName;
    // }

    // static void RDSRadioTextUpdate(const char *radioText)
    // {
    //     rdsRadioText = radioText;
    // }

    // static void RDSClear()
    // {
    //     rdsServiceName = "";
    //     rdsRadioText = "";
    // }

// System Functions
#pragma region Functions
    void ignitionOn()
    {
        DEBUG_LOG("Ignition on\n");

        ignitionState = true;
        stateBeforeIgnitionOn = systemState;

        updateSystemState(stateBeforeIgnitionOff);
    }

    void ignitionOff()
    {
        DEBUG_LOG("Ignition off\n");

        ignitionState = false;
        stateBeforeIgnitionOff = systemState;

        updateSystemState(SystemState::Off);
    }

    void togglePower()
    {
        DEBUG_LOG("Toggle power\n");
        switch (systemState)
        {
        case SystemState::Off:
        case SystemState::Standby:
            updateSystemState(SystemState::On);
            break;
        case SystemState::On:
            if (ignitionState)
            {
                updateSystemState(SystemState::Standby);
            }
            else
            {
                updateSystemState(SystemState::Off);
            }
            break;
        }
    }

    void displayFullDate()
    {
    }

    // The clock button
    void enterClockSetMode()
    {
        DEBUG_LOG("Clock set button\n");

        if (systemState != SystemState::Standby)
        {
            return;
        }

        switch (systemMode)
        {
        default:
        case SystemMode::Idle:
            updateSystemMode(SystemMode::DateSet);
            break;
        case SystemMode::DateSet:
            updateSystemMode(SystemMode::MonthSet);
            break;
        case SystemMode::MonthSet:
            updateSystemMode(SystemMode::YearSet);
            break;
        case SystemMode::YearSet:
            updateSystemMode(SystemMode::HourSet);
            break;
        case SystemMode::HourSet:
            updateSystemMode(SystemMode::MinuteSet);
            break;
        case SystemMode::MinuteSet:
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    // The circle button
    void clockFunction()
    {
        DEBUG_LOG("Clock Function button\n");

        uint8_t newHour;
        uint8_t newMinute;
        uint16_t newYear;
        uint8_t newMonth;
        uint8_t newDay;
        DateTime newDt;

        switch (systemState)
        {
        case SystemState::Off:
            break;
        case SystemState::On:
            updateSystemMode(SystemMode::ShowFullDateTime);
            break;
        case SystemState::Standby:
            DateTime now = rtc->now();
            switch (systemMode)
            {
            case SystemMode::Idle:
                // TODO: change temperature Mode
                break;
            case SystemMode::MinuteSet:
                newMinute = now.minute() < 59 ? now.minute() + 1 : 0;
                rtc->adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), newMinute, 0));
                updateSystemMode(SystemMode::MinuteSet);
                break;
            case SystemMode::HourSet:
                newHour = now.hour() < 23 ? now.hour() + 1 : 0;
                rtc->adjust(DateTime(now.year(), now.month(), now.day(), newHour, now.minute(), 0));
                updateSystemMode(SystemMode::HourSet);
                break;
            case SystemMode::DateSet:
                newDay = now.day() < 31 ? now.day() + 1 : 1;

                newDt = DateTime(now.year(), now.month(), newDay, now.hour(), now.minute(), 0);
                if (!newDt.isValid())
                {
                    newDt = DateTime(now.year(), now.month(), 1, now.hour(), now.minute(), 0);
                }

                rtc->adjust(newDt);
                updateSystemMode(SystemMode::DateSet);
                break;
            case SystemMode::MonthSet:
                newMonth = now.month() < 12 ? now.month() + 1 : 1;
                newDay = now.day();

                newDt = DateTime(now.year(), newMonth, newDay, now.hour(), now.minute(), 0);

                // If not valid then the days available in this month may not be the same as the previous month, so find a valid date
                while (!newDt.isValid())
                {
                    if (newDay == 1)
                        break;

                    newDt = DateTime(now.year(), newMonth, --newDay, now.hour(), now.minute(), 0);
                }

                rtc->adjust(newDt);
                updateSystemMode(SystemMode::MonthSet);
                break;
            case SystemMode::YearSet:
                newYear = now.year() < 2050 ? now.year() + 1 : 2001;
                newDay = now.day();

                // If not valid then the days available in this month may not be the same as the previous month, so find a valid date
                while (!newDt.isValid())
                {
                    if (newDay == 1)
                        break;

                    newDt = DateTime(now.year(), now.month(), --newDay, now.hour(), now.minute(), 0);
                }

                rtc->adjust(DateTime(now.year() + 1, now.month(), now.day(), now.hour(), now.minute(), 0));
                updateSystemMode(SystemMode::YearSet);
                break;
            default:
                break;
            }
            break;
        }
    }

    void increaseVolume()
    {
        if (systemState != SystemState::On)
        {
            return;
        }
        DEBUG_LOG("Volume increase\n");

        switch(systemMode)
        {
            case SystemMode::Treble:
                tda->treble(tda->treble() + 1);
                updateSystemMode(SystemMode::Treble);
                break;
            case SystemMode::Bass:
                tda->bass(tda->bass() + 1);
                updateSystemMode(SystemMode::Bass);
                break;
            default:
                tda->volume(tda->volume() + 1);
                updateSystemMode(SystemMode::Volume);
                break;
        }
        
    }

    void decreaseVolume()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Volume decrease\n");

        switch (systemMode)
        {
        case SystemMode::Treble:
            tda->treble(tda->treble() - 1);
            updateSystemMode(SystemMode::Treble);
            break;
        case SystemMode::Bass:
            tda->bass(tda->bass() - 1);
            updateSystemMode(SystemMode::Bass);
            break;
        default:
            tda->volume(tda->volume() - 1);
            updateSystemMode(SystemMode::Volume);
            break;
        }
    }

    void openBassAndBalanceSettings()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Bass/Balance\n");

        switch (systemMode)
        {
        case SystemMode::Bass:
            updateSystemMode(SystemMode::Balance);
            break;
        case SystemMode::Balance:
            updateSystemMode(SystemMode::Bass);
            break;
        default:
            updateSystemMode(SystemMode::Bass);
            break;
        }
    }

    void openTrebleAndFadeSettings()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Treble/Fader\n");

        switch (systemMode)
        {
        case SystemMode::Treble:
            updateSystemMode(SystemMode::Fader);
            break;
        case SystemMode::Fader:
            updateSystemMode(SystemMode::Treble);
            break;
        default:
            updateSystemMode(SystemMode::Treble);
            break;
        }
    }

    void toggleBassBoost()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Toggle Bass Boost\n");
    }

    void toggleLoudness()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Boost/Loudness\n");

        tda->loud(!tda->loud());
        updateSystemMode(SystemMode::Loudness);
    }

    void toggleTrafficAnnouncements()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Toggle Traffic Announcements\n");
    }

    void toggleSeekMode()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Seek Mode Toggle\n");

        switch (AudioSource::Radio)
        {
        case AudioSource::Radio:
            if (seekMode == RadioSeekMode::Auto)
                seekMode = RadioSeekMode::Manual;
            else
                seekMode = RadioSeekMode::Auto;
            updateSystemMode(SystemMode::SeekModeSet);
            break;
        }
    }

    void onDownBtn()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Seek Down\n");

        switch (audioSource)
        {
        case AudioSource::Radio:
#ifdef RADIO_ENABLED
            if (seekMode == RadioSeekMode::Auto)
                radio.seekDown();
            else
                radio.setFrequency(radio.getFrequency() - 10);
#endif
            isCurrentStationSaved = false;
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void onUpBtn()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Seek Up\n");

        switch (audioSource)
        {
        case AudioSource::Radio:
#ifdef RADIO_ENABLED
            if (seekMode == RadioSeekMode::Auto)
                radio.seekUp();
            else
                radio.setFrequency(radio.getFrequency() + 10);
#endif
            isCurrentStationSaved = false;
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void selectNextInput()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Next Input\n");

        switch (audioSource)
        {
        case AudioSource::Radio:
            switch (band)
            {
            case FMBand::FM1:
                band = FMBand::FM2;
                break;
            default:
                band = FMBand::FM1;
                audioSource = AudioSource::Aux;
                tda->input(AUDIO_IN_AUX);
                break;
            }
            break;
        case AudioSource::Aux:
            audioSource = AudioSource::Bluetooth;
            tda->input(AUDIO_IN_BT_USB_SD);
            break;
        case AudioSource::Bluetooth:
            audioSource = AudioSource::Radio;
            tda->input(AUDIO_IN_RADIO);
            break;

            // TODO: Add functionality for these
            // case AudioSource::USB:
            //     audioSource = AudioSource::Aux;
            //     break;
            // case AudioSource::SD:
            //     audioSource = AudioSource::USB;
            //     break;
        }
        updateSystemMode(SystemMode::InputSelection);
    }

    void findBestStations()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("BST\n");
    }

    void switchToAutomaticStationsBand()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("AST\n");
    }

    void selectStationN1()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 1\n");

        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(0);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN1()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(0);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }

    void selectStationN2()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 2\n");

        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(1);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN2()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(1);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }

    void selectStationN3()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 3\n");
        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(2);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN3()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(2);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }

    void selectStationN4()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 4\n");
        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(3);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN4()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(3);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }

    void selectStationN5()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 5\n");
        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(4);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN5()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(4);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }

    void selectStationN6()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        DEBUG_LOG("Station 6\n");
        switch (audioSource)
        {
        case AudioSource::Radio:
            tuneToSavedRadioStation(5);
            updateSystemMode(SystemMode::Idle);
            break;
        }
    }

    void saveStationToN6()
    {
        if (systemState != SystemState::On)
        {
            return;
        }

        switch (audioSource)
        {
        case AudioSource::Radio:
            storeRadioStation(5);
            updateSystemMode(SystemMode::StationSave);
            break;
        }
    }
#pragma endregion
};

#endif