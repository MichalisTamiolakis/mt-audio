#include <Arduino.h>
#include <WiFi.h>

#include <System.h>

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
#define VOLUME_INCREASE 0b10010
#define VOLUME_DECREASE 0b11000
#define BAS_BAL 0b00010
#define TRE_FAD 0b10000
#define BST_LDN 0b01100
#define TA 0b01001
#define DOWN 0b00101
#define UP 0b10100
#define BND_MAN 0b00100
#define AST 0b00111
#define N1 0b10001
#define N2 0b01010
#define N3 0b00110
#define N4 0b00011
#define N5 0b01000
#define N6 0b00001

// Ignition Sensing pin
#define S_IGNITION 32

System radioSystem = System();

// All the buttons
SingleButton *powerBtn;
SingleButton *clockSetBtn;
SingleButton *clockFuncBtn;
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

bool previousIgnitionState;

#pragma region ButtonCallbacks
void ignitionOn()
{
    radioSystem.ignitionOn();
}

void ignitionOff()
{
    radioSystem.ignitionOff();
}

void togglePower()
{
    radioSystem.togglePower();
}

void onClockSetBtn()
{
    radioSystem.enterClockSetMode();
}

void onDisplayFullDateButton()
{
    radioSystem.displayFullDate();
}

void onClockFuncBtn()
{
    radioSystem.clockFunction();
}

void increaseVolume()
{
    radioSystem.increaseVolume();
}

void decreaseVolume()
{
    radioSystem.decreaseVolume();
}

void openBassAndBalanceSettings()
{
    radioSystem.openBassAndBalanceSettings();
}

void openTrebleAndFadeSettings()
{
    radioSystem.openTrebleAndFadeSettings();
}

void toggleBassBoost()
{
    radioSystem.selectBestStationsBand();
}

void toggleLoudness()
{
    radioSystem.toggleLoudness();
}

void toggleTraficAnnouncements()
{
    radioSystem.toggleTrafficAnnouncements();
}

void onDownBtn()
{
    radioSystem.onDownBtn();
}

void onUpBtn()
{
    radioSystem.onUpBtn();
}

void toggleSeekMode()
{
    radioSystem.toggleSeekMode();
}

void selectNextInput()
{
    radioSystem.selectNextInput();
}

void findBestStations()
{
    radioSystem.findBestStations();
}

void switchToAutomaticStationsBand()
{
    radioSystem.switchToAutomaticStationsBand();
}

void selectStationN1()
{
    radioSystem.selectStationN1();
}

void saveStationToN1()
{
    radioSystem.saveStationToN1();
}

void selectStationN2()
{
    radioSystem.selectStationN2();
}

void saveStationToN2()
{
    radioSystem.saveStationToN2();
}

void selectStationN3()
{
    radioSystem.selectStationN3();
}

void saveStationToN3()
{
    radioSystem.saveStationToN3();
}

void selectStationN4()
{
    radioSystem.selectStationN4();
}

void saveStationToN4()
{
    radioSystem.saveStationToN4();
}

void selectStationN5()
{
    radioSystem.selectStationN5();
}

void saveStationToN5()
{
    radioSystem.saveStationToN5();
}

void selectStationN6()
{
    radioSystem.selectStationN6();
}

void saveStationToN6()
{
    radioSystem.saveStationToN6();
}
#pragma endregion

void initButtons()
{
    powerBtn = new SingleButton(BUTTON_SINGLE, PWR_BTN, 50, 300);
    powerBtn->onPress(togglePower);

    clockSetBtn = new SingleButton(BUTTON_LONG, CLK_BTN_SET, 50, 300);
    clockSetBtn->onPress(onDisplayFullDateButton);
    clockSetBtn->onLongPress(onClockSetBtn);

    clockFuncBtn = new SingleButton(BUTTON_REPEAT, CLK_BTN_OK, 50, 300);
    clockFuncBtn->onPress(onClockFuncBtn);

    volumeUpBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_INCREASE, 50, 300);
    volumeUpBtn->onPress(increaseVolume);

    volumeDownBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, VOLUME_DECREASE, 50, 300);
    volumeDownBtn->onPress(decreaseVolume);

    basBalBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BAS_BAL, 50, 300);
    basBalBtn->onPress(openBassAndBalanceSettings);

    treFadBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TRE_FAD, 50, 300);
    treFadBtn->onPress(openTrebleAndFadeSettings);

    bstLdnBtn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BST_LDN, 50, 300);
    bstLdnBtn->onPress(switchToAutomaticStationsBand);
    bstLdnBtn->onLongPress(toggleLoudness);

    taBtn = new MultiplexedButton(BUTTON_SINGLE, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, TA, 50, 300);
    taBtn->onPress(toggleTraficAnnouncements);

    downBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, DOWN, 50, 300);
    downBtn->onPress(onDownBtn);

    upBtn = new MultiplexedButton(BUTTON_REPEAT, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, UP, 50, 300);
    upBtn->onPress(onUpBtn);

    bandManBtn = new MultiplexedButton(BUTTON_LONG, BTN_B0, BTN_B1, BTN_B2, BTN_B3, BTN_B4, BND_MAN, 50, 300);
    bandManBtn->onPress(selectNextInput);
    bandManBtn->onLongPress(toggleSeekMode);

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
}

void updateButtons()
{
    // Update buttons
    powerBtn->loop();
    clockFuncBtn->loop();
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
}

void setup()
{
    pinMode(S_IGNITION, INPUT_PULLDOWN);

    delay(1000);
    radioSystem.init();

    delay(1000);
    initButtons();
}

void loop()
{
    // Ignition state change check
    if (digitalRead(S_IGNITION) != previousIgnitionState)
    {
        delay(300); // Crude debounce
        bool ignitionState = digitalRead(S_IGNITION);
        if (ignitionState != previousIgnitionState)
        {
            previousIgnitionState = ignitionState;

            if (ignitionState)
            {
                ignitionOn();
            }
            else
            {
                ignitionOff();
            }
        }
    }

    radioSystem.update();
    updateButtons();
}