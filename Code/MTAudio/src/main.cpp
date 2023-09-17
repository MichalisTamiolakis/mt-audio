#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ButtonHandler.h>

// Output
#define PWR_ENABLE 15
#define SCREEN_BACKLIGHT 2
#define BTN_BACKLIGHT 4
#define RADIO_RST 19
#define SDA 21
#define SCL 22

// Input
#define S_IGNITION 32
#define S_LIGHT 35
#define S_TEMPERATURE 34

// Function Declarations
void onPowerBtn();
void powerOn();
void powerOff();

ButtonHandler buttons(100, true, 0);

enum class SystemState
{
    On,
    Off,
};

SystemState state = SystemState::Off;

void setup()
{
    // Output
    pinMode(PWR_ENABLE, OUTPUT);
    pinMode(SCREEN_BACKLIGHT, OUTPUT);
    pinMode(BTN_BACKLIGHT, OUTPUT);

    buttons.init();

    buttons.onPower(onPowerBtn);
    buttons.onVolumeIncrease(onPowerBtn);
}

void loop()
{
    buttons.update();
}

void powerOn()
{
    digitalWrite(PWR_ENABLE, HIGH);
    analogWrite(SCREEN_BACKLIGHT, 255);
    analogWrite(BTN_BACKLIGHT, 255);
}

void powerOff()
{
    digitalWrite(PWR_ENABLE, LOW);
    analogWrite(SCREEN_BACKLIGHT, 0);
    analogWrite(BTN_BACKLIGHT, 0);
}

// Button Inputs
void onPowerBtn()
{
    if(state == SystemState::Off)
    {
        state = SystemState::On;
        powerOn();
    }
    else
    {
        state = SystemState::Off;
        powerOff();
    }
}

void onVolumeIncreaseBtn()
{
}

void onVolumeDecreaseBtn()
{
}