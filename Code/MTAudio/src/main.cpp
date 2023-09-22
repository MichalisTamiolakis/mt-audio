#include <Arduino.h>
#include <SingleButton.h>
#include <MultiplexedButton.h>
#include <FSM.h>

// LCD
#include <DisplayManager.h>

// SI4703
#include <radio.h>
// #include <RDSParser.h>
#include <SI4703.h>

// TDA7313
#include <Tda7313.h>

// RTC
// #include <RTClib.h>

// TDA ADDRESS
#define TDA_ADDRESS 0x44
#define LCD_ADDRESS 0x3F

// Output
#define PWR_ENABLE 15
#define BTN_BACKLIGHT 4
#define SCREEN_BACKLIGHT 2
#define RADIO_RST 19
#define SDA 21
#define SCL 22

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
#define AUDIO_IN_BT_USB_SD 1
#define AUDIO_IN_AUX 3

// Function Declarations
void onIgnitionOn();
void onIgnitionOff();
void onPowerBtn();
void onClockSetBtn();
void onClockOkBtn();
void onVolumeIncreaseBtn();
void onVolumeDecreaseBtn();
void onBasBalBtn();
void onTreFadBtn();
void onBstLdnBtn();
void onTaBtn();
void onDownBtn();
void onUpBtn();
void onBandManBtn();
void onAstBtn();
void onN1Btn();
void onN2Btn();
void onN3Btn();
void onN4Btn();
void onN5Btn();
void onN6Btn();

void onSystemStateChanged(SystemState newState, SystemState prevState);
void onSystemModeChanged(SystemMode newMode, SystemMode prevMode);
void onAudioSourceChanged(AudioSource newSource, AudioSource prevSource);

void processRDS(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);
void updateRDS(const char* stationName);

// FSM state changes
FSM fsm = FSM();

// Display
DisplayManager* display;

// All the buttons
SingleButton* powerBtn;
SingleButton* clockSetBtn;
SingleButton* clockOkBtn;
MultiplexedButton* volumeUpBtn;
MultiplexedButton* volumeDownBtn;
MultiplexedButton* basBalBtn;
MultiplexedButton* treFadBtn;
MultiplexedButton* bstLdnBtn;
MultiplexedButton* taBtn;
MultiplexedButton* downBtn;
MultiplexedButton* upBtn;
MultiplexedButton* bandManBtn;
MultiplexedButton* astBtn;
MultiplexedButton* n1Btn;
MultiplexedButton* n2Btn;
MultiplexedButton* n3Btn;
MultiplexedButton* n4Btn;
MultiplexedButton* n5Btn;
MultiplexedButton* n6Btn;

// Radio
// SI4703 radio = SI4703();
// RDSParser rds = RDSParser();

// TDA7313
Tda7313 tda = Tda7313(TDA_ADDRESS);

SystemState stateAtIgnitionOff = SystemState::Standby;
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

    // Init FSM, display and buttons
#pragma region  FSM
    fsm.onSystemStateChanged(onSystemStateChanged);
    fsm.onSystemModeChanged(onSystemModeChanged);
    fsm.onAudioSourceChanged(onAudioSourceChanged);
#pragma endregion

#pragma region Display
display = new DisplayManager(LCD_ADDRESS, BTN_BACKLIGHT);
#pragma endregion

#pragma region Buttons
    powerBtn = new SingleButton(PWR_BTN, 50, 1000, false);
    powerBtn->onPress(onPowerBtn);

    clockSetBtn = new SingleButton(CLK_BTN_SET, 50, 1000, false);
    clockSetBtn->onPress(onClockSetBtn);

    clockOkBtn = new SingleButton(CLK_BTN_OK, 50, 1000, false);
    clockOkBtn->onPress(onClockOkBtn);
    
    volumeUpBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_INCREASE, 50, 1000, true);
    volumeUpBtn->onPress(onVolumeIncreaseBtn);

    volumeDownBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_DECREASE, 50, 1000, true);
    volumeDownBtn->onPress(onVolumeDecreaseBtn);

    basBalBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BAS_BAL, 50, 1000, false);
    basBalBtn->onPress(onBasBalBtn);

    treFadBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TRE_FAD, 50, 1000, false);
    treFadBtn->onPress(onTreFadBtn);

    bstLdnBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BST_LDN, 50, 1000, false);
    bstLdnBtn->onPress(onBstLdnBtn);

    taBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TA, 50, 1000, false);
    taBtn->onPress(onTaBtn);

    downBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, DOWN, 50, 1000, false);
    downBtn->onPress(onDownBtn);

    upBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, UP, 50, 1000, false);
    upBtn->onPress(onUpBtn);

    bandManBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BND_MAN, 50, 1000, false);
    bandManBtn->onPress(onBandManBtn);

    astBtn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, AST, 50, 1000, false);
    astBtn->onPress(onAstBtn);

    n1Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N1, 50, 1000, false);
    n1Btn->onPress(onN1Btn);

    n2Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N2, 50, 1000, false);
    n2Btn->onPress(onN2Btn);

    n3Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N3, 50, 1000, false);
    n3Btn->onPress(onN3Btn);

    n4Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N4, 50, 1000, false);
    n4Btn->onPress(onN4Btn);

    n5Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N5, 50, 1000, false);
    n5Btn->onPress(onN5Btn);

    n6Btn = new MultiplexedButton(BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, N6, 50, 1000, false);
    n6Btn->onPress(onN6Btn);

#pragma endregion

#pragma region SI4703
    // radio.setup(RADIO_RESETPIN, RADIO_RST);
    // radio.init();
    // radio.setBandFrequency(RADIO_BAND_FM, 8870);
    // radio.setVolume(14);
    // radio.setBassBoost(false);
    // radio.setSoftMute(true);
    // radio.setMono(false);
    // radio.setMute(false);
    // rds.init();
    // //radio.debugEnable();
    // radio.attachReceiveRDS(processRDS);
    // rds.attachServiceNameCallback(updateRDS);
#pragma endregion

#pragma region TDA7313
    tda.input(AUDIO_IN_RADIO);	    // 1,2,3    Stereo 1, Stereo 2, Stereo 3
    tda.loud(false);	            // Loud off (true,false)
    tda.mute(false);                // Mute off (true,false)
    tda.volume(10);	                //  0.......15  -78.75dB...........0dB
    tda.bass(7);	                // 0....7...14  -14dB.....0dB....+14dB
    tda.treble(7);	                // 0....7...14  -14dB.....0dB....+14dB

    tda.sla(0);                     // 0,1,2,3  0dB, +3.75dB, +7.5dB, +11.25dB

    tda.attLF(0);	                //  0.......13  0dB.........-36.25dB
    tda.attRF(0);	                //  0.......13  0dB.........-36.25dB
    tda.attLR(0);	                //  0.......13  0dB.........-36.25dB
    tda.attRR(0);	                //  0.......13  0dB.........-36.25dB
#pragma endregion

    Serial.begin(115200);
    Wire.setClock(100000);

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
            digitalWrite(PWR_ENABLE, LOW);
            analogWrite(BTN_BACKLIGHT, 0);
            analogWrite(SCREEN_BACKLIGHT, 0);
            fsm.changeSystemMode(SystemMode::Idle);
            display->powerOff();
            break;
        case SystemState::Standby:
            digitalWrite(PWR_ENABLE, LOW);
            analogWrite(BTN_BACKLIGHT, 0);
            analogWrite(SCREEN_BACKLIGHT, 120);
            fsm.changeSystemMode(SystemMode::Idle);
            display->powerOn();
            break;
        case SystemState::On:
            digitalWrite(PWR_ENABLE, HIGH);
            analogWrite(BTN_BACKLIGHT, 120);
            analogWrite(SCREEN_BACKLIGHT, 120);
            display->powerOn();
            fsm.changeSystemMode(SystemMode::TurnOnSequence);
            delay(500);
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
            display->displayIdle(fsm.getAudioSource());
            display->updateTime(0);
            display->updateTemperature(0);
            break;
        case SystemMode::Volume:
            display->displayVolume();
            break;
    }
}

void onAudioSourceChanged(AudioSource newSource, AudioSource prevSource)
{
    fsm.print();

    display->displayIdle(newSource);

    switch (newSource)
    {
        case AudioSource::FM1:
        case AudioSource::FM2:
        case AudioSource::FMBst:
            tda.input(AUDIO_IN_RADIO);
            break;
        case AudioSource::Aux:
            tda.input(AUDIO_IN_AUX);
            break;
        case AudioSource::Bluetooth:
        case AudioSource::USB:
        case AudioSource::SD:
            tda.input(AUDIO_IN_BT_USB_SD);
            break;
    }
}
#pragma endregion

// Button/Ignition Callbacks and FSM state changes
#pragma region ButtonCallbacks
void onIgnitionOn()
{
    Serial.println("Ignition on");

    // If already on or in standby, do nothing
    if(fsm.getSystemState() == SystemState::Off)
        fsm.changeSystemState(stateAtIgnitionOff);
}

void onIgnitionOff()
{
    Serial.println("Ignition off");
    stateAtIgnitionOff = fsm.getSystemState();
}

void onPowerBtn()
{
    Serial.println("Power button");

    switch (fsm.getSystemState())
    {
        case SystemState::Off:
        case SystemState::Standby:
            fsm.changeSystemState(SystemState::On);
            break;
        case SystemState::On:
            if(digitalRead(S_IGNITION))
            {
                fsm.changeSystemState(SystemState::Standby);
            }
            else
            {
                fsm.changeSystemState(SystemState::Off);
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

void onVolumeIncreaseBtn()
{
    // Change mode 
    if(fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Volume increase");
        fsm.changeSystemMode(SystemMode::Volume);
        
        // Increase volume update display
        tda.volume(tda.volume() + 1);
        display->updateVolume(tda.volume());
    }
}

void onVolumeDecreaseBtn()
{
    // Change mode 
    if(fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Volume decrease");
        fsm.changeSystemMode(SystemMode::Volume);
        
        // Decrease volume update display
        tda.volume(tda.volume() - 1);
        display->updateVolume(tda.volume());
    }
}

void onBasBalBtn()
{
    Serial.println("Bass/Balance");
}

void onTreFadBtn()
{
    Serial.println("Treble/Fader");
}

void onBstLdnBtn()
{
    Serial.println("Boost/Loudness");
}

void onTaBtn()
{
    Serial.println("TA");
}

void onDownBtn()
{

    if(fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Down");
        // radio.seekDown();
    }
}

void onUpBtn()
{

    if(fsm.getSystemState() == SystemState::On)
    {
        Serial.println("Up");
        // radio.seekUp();
    }
}

void onBandManBtn()
{
    Serial.println("Band/Manual");
    if(fsm.getSystemState() == SystemState::On)
    {
        switch (fsm.getAudioSource())
        {
            case AudioSource::FM1:
                fsm.changeAudioSource(AudioSource::FM2);
                break;
            case AudioSource::FM2:
                fsm.changeAudioSource(AudioSource::Aux);
                break;
            case AudioSource::FMBst:
                fsm.changeAudioSource(AudioSource::FM1);
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
        }
    }
}

void onAstBtn()
{
    Serial.println("AST");
}   

void onN1Btn()
{
    Serial.println("1");
}   

void onN2Btn()
{
    Serial.println("2");
}   

void onN3Btn()
{
    Serial.println("3");
}   

void onN4Btn()
{
    Serial.println("4");
}

void onN5Btn()
{
    Serial.println("5");
}

void onN6Btn()
{
    Serial.println("6");
}
#pragma endregion

#pragma region RDS Callbacks
// void updateRDS(const char* stationName){ //updates display When new RDS data is available
//     // display.
// }

// void processRDS(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4){ //Gets the data of RDS and process it
//   rds.processData(block1, block2, block3, block4);
// }
#pragma endregion