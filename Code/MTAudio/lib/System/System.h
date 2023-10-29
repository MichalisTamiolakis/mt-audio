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

// SI4703
#include <radio.h>
#include <SI4703.h>
#include <RDSParser.h>

// TDA7313
#include <Tda7313.h>

// BT201
#include <BT201.h>

#define DEBUG_LOG_ENABLED

#ifdef DEBUG_LOG_ENABLED
#define DEBUG_LOG(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define DEBUG_LOG(format, ...)
#endif

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

class System
{
private:
    // Display
    DisplayManager *display;

    // Radio
    SI4703 *radio;
    // static RDSParser *rdsParser;
    // static String rdsServiceName;
    // static String rdsRadioText;
    uint16_t stationAtShutdown;
    uint16_t savedStations[3][6];

    // TDA7313
    Tda7313 *tda;
    BT201 *bt201;

    SystemState stateBeforeIgnitionOn;
    SystemState stateBeforeIgnitionOff;

    AsyncDelayHelper *delayHelper;
    bool hasDelayStarted;
    SystemMode delayMode; // Mode to switch to after delay

    void initRadio()
    {
        radio->setup(RADIO_RESETPIN, RADIO_RST);
        radio->setup(RADIO_MODEPIN, SDA_PIN);

        // radio.debugEnable(true); // Turns debug information on
        // radio._wireDebug(true);  // Turns I2C debug information on

        radio->setup(RADIO_FMSPACING, RADIO_FMSPACING_100);
        radio->setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);

        radio->initWire(Wire);

        // radio.debugEnable(true); // Turns debug information on
        // radio._wireDebug(true);  // Turns I2C debug information on

        radio->setBandFrequency(RADIO_BAND_FM, stationAtShutdown);
        radio->setVolume(15);
        radio->setMono(false);
        radio->setMute(false);
        radio->setSoftMute(true);

        // radio->attachReceiveRDS(RDSProcess);
        // rdsParser->attachServiceNameCallback(RDSServiceNameUpdate);
        // rdsParser->attachTextCallback(RDSRadioTextUpdate);
    }

    void shutdownRadio()
    {
        stationAtShutdown = radio->getFrequency();
        radio->setMute(true);
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

    void updateSystemState(SystemState newState)
    {
        SystemState prevState = systemState;
        systemState = newState;
        
        // State change functionality
        switch(newState)
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
                if(prevState == SystemState::On)
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
                if(prevState == SystemState::On)
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
    }

    void updateSystemMode(SystemMode newMode)
    {
        systemMode = newMode;

        // Do delayed transition stuff here.
        stopDelayedModeTransition();
        switch(newMode)
        {
            case SystemMode::TurnOnSequence:
                startDelayedModeTransition(SystemMode::Idle, TURN_ON_SEQUENCE_DISPLAY_TIME);
                break;

        }

        // Do display related stuff here.
        display->updateMainDisplay();
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

public:
    SystemState systemState;
    SystemMode systemMode;
    AudioSource audioSource;
    bool ignitionState;

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

        // Radio
        radio = new SI4703();
        // rdsParser = new RDSParser();

        // TDA7313
        tda = new Tda7313(TDA_ADDRESS);
        bt201 = new BT201(&Serial2);

        systemState = SystemState::Off;
        systemMode = SystemMode::Idle;
        audioSource = AudioSource::Radio;
        ignitionState = false;

        stateBeforeIgnitionOn = SystemState::Off;
        stateBeforeIgnitionOff = SystemState::On;
    }


    /// @brief Should be called in init, to initialize the buttons, display, etc.
    void init()
    {
        // Output
        pinMode(PWR_ENABLE, OUTPUT);
        pinMode(BTN_BACKLIGHT, OUTPUT);
        pinMode(RADIO_RST, OUTPUT);

        // Input sensors
        pinMode(S_TEMPERATURE, INPUT_PULLDOWN);
        pinMode(S_LIGHT, INPUT_PULLDOWN);

        Wire.begin(SDA_PIN, SCL_PIN);

        Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

        display = new DisplayManager(LCD_ADDRESS, BTN_BACKLIGHT);

        initRadio();
        initTDA();

        Serial.begin(115200);
        Wire.setClock(300000);
    }

    /// @brief Should be called every loop to update input, display, etc.
    void update()
    {
        radio->checkRDS();

        // Delay check
        delayHelper->loop();
        if (hasDelayStarted && delayHelper->isDelayFinished())
        {
            hasDelayStarted = false;
            updateSystemMode(delayMode);
        }
    }

    float getTemperature()
    {
        return 0.0f;
    }

    float getTime()
    {
        return 0.0f;
    }

    // FM Getter Functions
    float getFMFrequency()
    {
        return 0.0f;
    }

    FMBand getFMFrequencyBand()
    {
        return FMBand::FM;
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
        ignitionState = true;
        stateBeforeIgnitionOn = systemState;

        updateSystemState(stateBeforeIgnitionOff);

        // Turn ICs on here.
        initRadio();
        tda->sync();
    }

    void ignitionOff()
    {
        ignitionState = false;
        stateBeforeIgnitionOff = systemState;

        updateSystemState(SystemState::Off);
    }

    void togglePower()
    {
        switch(systemState)
        {
            case systemState::Off:
            case systemState::Idle:
                updateSystemState(SystemState::On);
                break;
            case systemState::On:
                if(ignitionState)
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

    void onClockSetBtn()
    {
        DEBUG_LOG("Clock set button");
    }

    void onClockOkBtn()
    {
        DEBUG_LOG("Clock ok button");
    }

    void increaseVolume()
    {
    }

    void decreaseVolume()
    {
        DEBUG_LOG("Volume decrease");
    }

    void openBassAndBalanceSettings()
    {
        DEBUG_LOG("Bass/Balance");
    }

    void openTrebleAndFadeSettings()
    {
        DEBUG_LOG("Treble/Fader");
    }

    void toggleBassBoost()
    {
    }

    void toggleLoudness()
    {
        DEBUG_LOG("Boost/Loudness");
    }

    void toggleTraficAnnouncements()
    {
        DEBUG_LOG("TA");
    }

    void onDownBtn()
    {
    }

    void onUpBtn()
    {
    }

    void selectNextInput()
    {
        DEBUG_LOG("Band");
    }

    void findBestStations()
    {
        DEBUG_LOG("AST");
    }

    void selectStationN1()
    {

    }

    void saveStationToN1()
    {

    }

    void selectStationN2()
    {

    }

    void saveStationToN2()
    {

    }

    void selectStationN3()
    {

    }

    void saveStationToN3()
    {

    }

    void selectStationN4()
    {

    }

    void saveStationToN4()
    {

    }

    void selectStationN5()
    {

    }

    void saveStationToN5()
    {

    }

    void selectStationN6()
    {

    }

    void saveStationToN6()
    {

    }
#pragma endregion
};

#endif