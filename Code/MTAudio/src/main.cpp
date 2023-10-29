#include <Arduino.h>
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

// Ignition Sensing pin
#define S_IGNITION 32


System radioSystem;

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

bool previousIgnitionState;


#pragma region  ButtonCallbacks
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
    radioSystem.onClockSetBtn();
}

void onClockOkBtn()
{
    radioSystem.onClockOkBtn();
}

void increaseVolume()
{
}

void decreaseVolume()
{
}

void openBassAndBalanceSettings()
{
}

void openTrebleAndFadeSettings()
{
}

void toggleBassBoost()
{
}

void toggleLoudness()
{
}

void toggleTraficAnnouncements()
{
}

void onDownBtn()
{
}

void onUpBtn()
{
}

void selectNextInput()
{
}

void findBestStations()
{
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

void initButtons()
{
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
}

void updateButtons()
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
}

void setup()
{
    pinMode(S_IGNITION, INPUT_PULLDOWN);
    radioSystem.init();
}

void loop()
{
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
                ignitionOn();
            }
            else
            {
                ignitionOff();
            }
        }
    }

    radioSystem.update();
}