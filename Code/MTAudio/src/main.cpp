#include <Arduino.h>
#include <SingleButton.h>
#include <MultiplexedButton.h>
#include <FSM.h>

// LCD
#include <DisplayManager.h>

// SI4703
#include <radio.h>
#include <SI4703.h>
#include <RDSParser.h>

// TDA7313
#include <Tda7313.h>

// BT201
#include <BT201.h>

// RTC
// #include <RTClib.h>

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
#define S_IGNITION 32
#define S_LIGHT 35
#define S_TEMPERATURE 34

// Button I/O pins
#define PWR_BTN 33
#define CLK_BTN_SET 25
#define CLK_BTN_OK 23
#define BTN_B0 13
#define BTN_B1 12
#define BTN_B2 14
#define BTN_B3 27
#define BTN_B4 26

// Multiplexed button codes
#define VOLUME_INCREASE 0x12
#define VOLUME_DECREASE 0x18
#define BAS_BAL 0x2
#define TRE_FAD 0x10
#define BST_LDN 0xC
#define TA 0x9
#define DOWN 0x5
#define UP 0x14
#define BND_MAN 0x4
#define AST 0x7
#define N1 0x11
#define N2 0xA
#define N3 0x6
#define N4 0x3
#define N5 0x8
#define N6 0x1

// Audio Sources
#define AUDIO_IN_RADIO 2
#define AUDIO_IN_BT_USB_SD 3
#define AUDIO_IN_AUX 1

// Generic Settings
#define SAVE_STATION_PAUSE 300 // 300 ms pause when saving station 

// Function Declarations
void onIgnitionOn();
void onIgnitionOff();
void togglePower();
void onClockSetBtn();
void onClockOkBtn();
void increaseVolume();
void decreaseVolume();
void openBassAndBalanceSettings();
void openTrebleAndFadeSettings();
void toggleLoudness();
void toggleTraficAnnouncements();
void onDownBtn();
void onUpBtn();
void selectNextInput();
void findBestStations();
void selectStationN1();
void saveStationToN1();
void selectStationN2();
void saveStationToN2();
void selectStationN3();
void saveStationToN3();
void selectStationN4();
void saveStationToN4();
void selectStationN5();
void saveStationToN5();
void selectStationN6();
void saveStationToN6();

void onSystemStateChanged(SystemState newState, SystemState prevState);
void onSystemModeChanged(SystemMode newMode, SystemMode prevMode);
void onAudioSourceChanged(AudioSource newSource, AudioSource prevSource);

// Radio
void InitRadio();
void StopRadio();
void RDSProcess(uint16_t blockA, uint16_t blockB, uint16_t blockC, uint16_t blockD);
void RDSServiceNameUpdate(const char *serviceName);
void RDSRadioTextUpdate(const char *radioText);
void RDSClear();

// FSM state changes
FSM fsm = FSM();

// Display
DisplayManager *display;

// All the buttons
SingleButton *powerBtn;
SingleButton *clockSetBtn;
SingleButton *clockOkBtn;
MultiplexedButton *volumeUpBtn;
MultiplexedButton *volumeDownBtn;
MultiplexedButton *basBalBtn;
MultiplexedButton *treFadBtn;
MultiplexedButton *bstLdnBtn;
MultiplexedButton *taBtn;
MultiplexedButton *downBtn;
MultiplexedButton *upBtn;
MultiplexedButton *bandManBtn;
MultiplexedButton *astBtn;
MultiplexedButton *n1Btn;
MultiplexedButton *n2Btn;
MultiplexedButton *n3Btn;
MultiplexedButton *n4Btn;
MultiplexedButton *n5Btn;
MultiplexedButton *n6Btn;

// Radio
SI4703 radio;
RDSParser rdsParser;
uint16_t stationAtShutdown = 8870;
uint16_t savedStations[3][6] =
    {
        {8870, 8870, 8870, 8870, 8870, 8870},
        {8870, 8870, 8870, 8870, 8870, 8870},
        {8870, 8870, 8870, 8870, 8870, 8870}};

// TDA7313
Tda7313 tda = Tda7313(TDA_ADDRESS);
BT201 bt201(&Serial2);

SystemState stateBeforeIgnitionOn = SystemState::Off;
SystemState stateBeforeIgnitionOff = SystemState::On;
bool previousIgnitionState = false;

void setup()
{
    // Output
    pinMode(PWR_ENABLE, OUTPUT);
    pinMode(BTN_BACKLIGHT, OUTPUT);
    pinMode(RADIO_RST, OUTPUT);

    // Input sensors
    pinMode(S_IGNITION, INPUT_PULLDOWN);
    pinMode(S_TEMPERATURE, INPUT_PULLDOWN);
    pinMode(S_LIGHT, INPUT_PULLDOWN);

    Wire.begin(SDA_PIN, SCL_PIN);

    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    // Init FSM, display and buttons
#pragma region FSM
    fsm.onSystemStateChanged(onSystemStateChanged);
    fsm.onSystemModeChanged(onSystemModeChanged);
    fsm.onAudioSourceChanged(onAudioSourceChanged);
#pragma endregion

#pragma region Display
    display = new DisplayManager(LCD_ADDRESS, BTN_BACKLIGHT);
#pragma endregion

#pragma region Buttons
    powerBtn = new SingleButton(BUTTON_SINGLE, PWR_BTN, 50, 300);
    powerBtn->onPress(togglePower);

    clockSetBtn = new SingleButton(BUTTON_SINGLE, CLK_BTN_SET, 50, 300);
    clockSetBtn->onPress(onClockSetBtn);

    clockOkBtn = new SingleButton(BUTTON_SINGLE, CLK_BTN_OK, 50, 300);
    clockOkBtn->onPress(onClockOkBtn);

    volumeUpBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_INCREASE, 50, 300);
    volumeUpBtn->onPress(increaseVolume);

    volumeDownBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_DECREASE, 50, 300);
    volumeDownBtn->onPress(decreaseVolume);

    basBalBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BAS_BAL, 50, 300);
    basBalBtn->onPress(openBassAndBalanceSettings);

    treFadBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TRE_FAD, 50, 300);
    treFadBtn->onPress(openTrebleAndFadeSettings);

    bstLdnBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BST_LDN, 50, 300);
    bstLdnBtn->onPress(toggleLoudness);

    taBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TA, 50, 300);
    taBtn->onPress(toggleTraficAnnouncements);

    downBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, DOWN, 50, 300);
    downBtn->onPress(onDownBtn);

    upBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, UP, 50, 300);
    upBtn->onPress(onUpBtn);

    bandManBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BND_MAN, 50, 300);
    bandManBtn->onPress(selectNextInput);

    astBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, AST, 50, 300);
    astBtn->onPress(findBestStations);

    n1Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N1, 50, 1000);
    n1Btn->onPress(selectStationN1);
    n1Btn->onLongPress(saveStationToN1);

    n2Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N2, 50, 1000);
    n2Btn->onPress(selectStationN2);
    n2Btn->onLongPress(saveStationToN2);

    n3Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N3, 50, 1000);
    n3Btn->onPress(selectStationN3);
    n3Btn->onLongPress(saveStationToN3);

    n4Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N4, 50, 1000);
    n4Btn->onPress(selectStationN4);
    n4Btn->onLongPress(saveStationToN4);

    n5Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N5, 50, 1000);
    n5Btn->onPress(selectStationN5);
    n5Btn->onLongPress(saveStationToN5);

    n6Btn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N6, 50, 1000);
    n6Btn->onPress(selectStationN6);
    n6Btn->onLongPress(saveStationToN6);

#pragma endregion

#pragma region SI4703
    InitRadio();
#pragma endregion

#pragma region TDA7313
    tda.input(AUDIO_IN_RADIO); // 1,2,3    Stereo 1, Stereo 2, Stereo 3
    tda.loud(false);           // Loud off (true,false)
    tda.mute(false);           // Mute off (true,false)
    tda.volume(7);            //  0.......15  -78.75dB...........0dB
    tda.bass(7);               // 0....7...14  -14dB.....0dB....+14dB
    tda.treble(7);             // 0....7...14  -14dB.....0dB....+14dB

    tda.sla(2); // 0,1,2,3  0dB, +3.75dB, +7.5dB, +11.25dB

    tda.attLF(0); //  0.......13  0dB.........-36.25dB
    tda.attRF(0); //  0.......13  0dB.........-36.25dB
    tda.attLR(0); //  0.......13  0dB.........-36.25dB
    tda.attRR(0); //  0.......13  0dB.........-36.25dB
#pragma endregion

    Serial.begin(115200);
    Wire.setClock(300000);
}

void loop()
{
    // Update buttons
    powerBtn->loop();
    clockOkBtn->loop();
    clockSetBtn->loop();
    volumeUpBtn->loop();
    volumeDownBtn->loop();
    basBalBtn->loop();
    treFadBtn->loop();
    bstLdnBtn->loop();
    taBtn->loop();
    downBtn->loop();
    upBtn->loop();
    bandManBtn->loop();
    astBtn->loop();
    n1Btn->loop();
    n2Btn->loop();
    n3Btn->loop();
    n4Btn->loop();
    n5Btn->loop();
    n6Btn->loop();

    fsm.loop();

    radio.checkRDS();

#pragma region Ignition state
    // Ignition state change check
    if (digitalRead(S_IGNITION) != previousIgnitionState)
    {
        delay(100); // Crude debounce
        bool ignitionState = digitalRead(S_IGNITION);
        if (ignitionState != previousIgnitionState)
        {
            previousIgnitionState = ignitionState;

            if (ignitionState)
            {
                onIgnitionOn();
            }
            else
            {
                onIgnitionOff();
            }
        }
    }
#pragma endregion
}

#pragma region FSM Callbacks
void onSystemStateChanged(SystemState newState, SystemState prevState)
{
    fsm.print();

    switch (newState)
    {
    case SystemState::Off:
        StopRadio();
        delay(100);
        digitalWrite(PWR_ENABLE, LOW);
        analogWrite(BTN_BACKLIGHT, 0);
        analogWrite(SCREEN_BACKLIGHT, 0);
        fsm.changeSystemMode(SystemMode::Idle);
        display->powerOff();
        break;
    case SystemState::Standby:
        display->powerOn();
        fsm.changeSystemMode(SystemMode::Idle);
        StopRadio();
        delay(100);
        digitalWrite(PWR_ENABLE, LOW);
        analogWrite(BTN_BACKLIGHT, 0);
        analogWrite(SCREEN_BACKLIGHT, 120);
        break;
    case SystemState::On:
        InitRadio();
        digitalWrite(PWR_ENABLE, HIGH);
        analogWrite(BTN_BACKLIGHT, 120);
        analogWrite(SCREEN_BACKLIGHT, 120);
        display->powerOn();
        fsm.changeSystemMode(SystemMode::TurnOnSequence);
        delay(100);
        tda.sync();
        break;
    }
}

void onSystemModeChanged(SystemMode newMode, SystemMode prevMode)
{
    fsm.print();

    switch (newMode)
    {
    case SystemMode::TurnOnSequence:
        display->displayTurnOn();
        break;
    case SystemMode::Idle:
        if(fsm.getSystemState() == SystemState::On)
        {
            display->displayIdle(fsm.getAudioSource());
            display->updateTime(0);
            display->updateTemperature(0);
            switch (fsm.getAudioSource())
            {
            case AudioSource::Radio:
                display->updateFM(radio.getFrequency(), fsm.isStationSaved? fsm.fmBand : FMBand::FM, nullptr, nullptr);
                break;
            }
        }
        else if(fsm.getSystemState() == SystemState::Standby)
        {
            display->displayStandbyIdle();
        }
        break;
    case SystemMode::Volume:
        display->displayVolume();
        break;
    case SystemMode::InputSelection:
        display->displayInputSelection();
        break;
    case SystemMode::StationSave:
        display->displayStationSave();
        break;
    }
}

void onAudioSourceChanged(AudioSource newSource, AudioSource prevSource)
{
    fsm.print();
    switch (newSource)
    {
    case AudioSource::Radio:
        tda.input(AUDIO_IN_RADIO);
        break;
    case AudioSource::Aux:
        tda.input(AUDIO_IN_AUX);
        break;
    case AudioSource::Bluetooth:
        tda.input(AUDIO_IN_BT_USB_SD);
    case AudioSource::USB:
        tda.input(AUDIO_IN_BT_USB_SD);
        // Check if there is usb to actually switch to it. Otherwise switch to SD.
        bt201.setAudioMode(AudioMode::UDisk);
        delay(100);
        AudioMode mode = bt201.getAudioMode();
        if(mode != AudioMode::UDisk) // Did not switch successfully
        {
            fsm.changeAudioSource(AudioSource::SD);
        }
    case AudioSource::SD:
        tda.input(AUDIO_IN_BT_USB_SD);

        // Check if there is sd card to actually switch to it. Otherwise switch back to Radio.
        bt201.setAudioMode(AudioMode::TFCard);
        delay(100);
        AudioMode mode = bt201.getAudioMode();
        if(mode != AudioMode::TFCard) // Did not switch successfully
        {
            fsm.changeAudioSource(AudioSource::Radio);
        }
        break;
    }
}
#pragma endregion

// Button/Ignition Callbacks and FSM state changes
#pragma region ButtonCallbacks
void onIgnitionOn()
{
    Serial.println("Ignition on");

    stateBeforeIgnitionOn = fsm.getSystemState();

    // If already on or in standby, do nothing
    if (fsm.getSystemState() == SystemState::Off)
        fsm.changeSystemState(stateBeforeIgnitionOff);
}

void onIgnitionOff()
{    
    Serial.println("Ignition off");

    stateBeforeIgnitionOff = fsm.getSystemState();

    // If at the time we turned on the ignition the radio was already and now is on, then keep it on.
    if(stateBeforeIgnitionOn == SystemState::On && fsm.getSystemState() == SystemState::On)
    {
    }

    // Else Turn off the radio completely
    else{
        fsm.changeSystemState(SystemState::Off);
    }
}

void togglePower()
{
    Serial.println("Power button");

    switch (fsm.getSystemState())
    {
    // Toggle to on
    case SystemState::Off:
    case SystemState::Standby:
        fsm.changeSystemState(SystemState::On);
        break;

    // Toggle to off
    case SystemState::On:
        if (digitalRead(S_IGNITION))
        {
            fsm.changeSystemState(SystemState::Standby);
        }
        else
        {
            fsm.changeSystemState(SystemState::Off);
            fsm.changeSystemMode(SystemMode::Idle);
        }
    }
}

void onClockSetBtn()
{
    Serial.println("Clock set button");
}

void onClockOkBtn()
{
    Serial.println("Clock ok button");
}

void increaseVolume()
{
    // Change mode
    if (fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Volume increase");
        fsm.changeSystemMode(SystemMode::Volume);

        // Increase volume update display
        tda.volume(tda.volume() + 1);
        display->updateVolume(tda.volume());
    }
}

void decreaseVolume()
{
    // Change mode
    if (fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Volume decrease");
        fsm.changeSystemMode(SystemMode::Volume);

        // Decrease volume update display
        tda.volume(tda.volume() - 1);
        display->updateVolume(tda.volume());
    }
}

void openBassAndBalanceSettings()
{
    Serial.println("Bass/Balance");
}

void openTrebleAndFadeSettings()
{
    Serial.println("Treble/Fader");
}

void toggleBassBoost()
{

}

void toggleLoudness()
{
    Serial.println("Boost/Loudness");
}

void toggleTraficAnnouncements()
{
    Serial.println("TA");
}

void onDownBtn()
{
    if (fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Down");
        switch (fsm.getAudioSource())
        {
        case AudioSource::Radio:
            switch (fsm.seekMode)
            {
            case SeekMode::Auto:
                radio.seekDown();
                break;
            case SeekMode::Manual:
                radio.setFrequency(radio.getFrequency() + radio.getFrequencyStep());
                break;
            }
            fsm.isStationSaved = false;
            fsm.changeSystemMode(SystemMode::Idle);
            RDSClear();
            display->updateFM(radio.getFrequency(), FMBand::FM, nullptr, nullptr);
            break;
        }
    }
}

void onUpBtn()
{
    if (fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Up");
        switch (fsm.getAudioSource())
        {
        case AudioSource::Radio:
            // radio.clearRdsBuffer();
            switch (fsm.seekMode)
            {
            case SeekMode::Auto:
                radio.seekUp();
                break;
            case SeekMode::Manual:
                radio.setFrequency(radio.getFrequency() - radio.getFrequencyStep());
                break;
            }
            fsm.isStationSaved = false;
            fsm.changeSystemMode(SystemMode::Idle);
            RDSClear();
            display->updateFM(radio.getFrequency(), FMBand::FM, nullptr, nullptr);
            break;
        }
    }
}

void selectNextInput()
{
    Serial.println("Band");
    if (fsm.getSystemState() == SystemState::On)
    {
        fsm.changeSystemMode(SystemMode::InputSelection);
        switch (fsm.getAudioSource())
        {
        case AudioSource::Radio:
            switch(fsm.fmBand)
            {
                case FMBand::FM:
                    fsm.fmBand = FMBand::FM1;
                    break;
                case FMBand::FMBst:
                    fsm.fmBand = FMBand::FM1;
                    break;
                case FMBand::FM1:
                    fsm.fmBand = FMBand::FM2;
                    break;
                case FMBand::FM2:
                    fsm.changeAudioSource(AudioSource::Aux);
                    break;
            }
            break;
        case AudioSource::Aux:
            fsm.changeAudioSource(AudioSource::Bluetooth);
            break;
        case AudioSource::Bluetooth:
            fsm.changeAudioSource(AudioSource::USB);
            break;
        case AudioSource::USB:
            fsm.changeAudioSource(AudioSource::SD);
            break;
        case AudioSource::SD:
            fsm.fmBand = FMBand::FM1;
            fsm.changeAudioSource(AudioSource::Radio);
            break;
        }
        display->updateInputSelection(fsm.getAudioSource(), fsm.fmBand);
    }
}

void findBestStations()
{
    Serial.println("AST");
}

void selectStationN1()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][0]);
}

void saveStationToN1()
{
    savedStations[(int)fsm.fmBand][0] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}

void selectStationN2()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][1]);
}

void saveStationToN2()
{
    savedStations[(int)fsm.fmBand][1] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}

void selectStationN3()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][2]);
}

void saveStationToN3()
{
    savedStations[(int)fsm.fmBand][2] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}

void selectStationN4()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][3]);
}

void saveStationToN4()
{
    savedStations[(int)fsm.fmBand][3] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}

void selectStationN5()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][4]);
}

void saveStationToN5()
{
    savedStations[(int)fsm.fmBand][4] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}

void selectStationN6()
{
    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::Idle);

    radio.setFrequency(savedStations[(int)fsm.fmBand][5]);
}

void saveStationToN6()
{
    savedStations[(int)fsm.fmBand][5] = radio.getFrequency();
    
    tda.mute(true);
    delay(SAVE_STATION_PAUSE);
    tda.mute(false);

    fsm.isStationSaved = true;
    fsm.changeSystemMode(SystemMode::StationSave);
    display->updateStationSave(radio.getFrequency(), fsm.fmBand);
}
#pragma endregion

#pragma regios Radio
void InitRadio()
{
    radio.setup(RADIO_RESETPIN, RADIO_RST);
    radio.setup(RADIO_MODEPIN, SDA_PIN);

    // radio.debugEnable(true); // Turns debug information on
    // radio._wireDebug(true);  // Turns I2C debug information on

    radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);
    radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);

    radio.initWire(Wire);

    // radio.debugEnable(true); // Turns debug information on
    // radio._wireDebug(true);  // Turns I2C debug information on

    radio.setBandFrequency(RADIO_BAND_FM, stationAtShutdown);
    radio.setVolume(15);
    radio.setMono(false);
    radio.setMute(false);

    radio.attachReceiveRDS(RDSProcess);
    rdsParser.attachServiceNameCallback(RDSServiceNameUpdate);
    rdsParser.attachTextCallback(RDSRadioTextUpdate);
}

void StopRadio()
{
    stationAtShutdown = radio.getFrequency();
}
#pragma endregion

#pragma region RDS Callbacks
void RDSProcess(uint16_t blockA, uint16_t blockB, uint16_t blockC, uint16_t blockD)
{
    rdsParser.processData(blockA, blockB, blockC, blockD);
}

void RDSServiceNameUpdate(const char *serviceName)
{
    // display.updateRDSServiceName(serviceName);
    if (fsm.getSystemState() == SystemState::On && fsm.getSystemMode() == SystemMode::Idle && fsm.getAudioSource() == AudioSource::Radio)
    {
        display->updateFM(radio.getFrequency(), fsm.isStationSaved? fsm.fmBand : FMBand::FM, serviceName, nullptr);
    }
}

void RDSRadioTextUpdate(const char *radioText)
{
    if (fsm.getSystemState() == SystemState::On && fsm.getSystemMode() == SystemMode::Idle && fsm.getAudioSource() == AudioSource::Radio)
    {
        display->updateFM(radio.getFrequency(), fsm.isStationSaved? fsm.fmBand : FMBand::FM, nullptr, radioText);
    }
}

void RDSClear()
{
    if (fsm.getAudioSource() == AudioSource::Radio)
    {
        display->updateFM(radio.getFrequency(), fsm.isStationSaved? fsm.fmBand : FMBand::FM, nullptr, nullptr);
    }
}
#pragma endregion