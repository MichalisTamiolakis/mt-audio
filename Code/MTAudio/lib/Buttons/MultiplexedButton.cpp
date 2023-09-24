#include "MultiplexedButton.h"

MultiplexedButton::MultiplexedButton(uint8_t buttonType, uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t buttonCode, uint32_t debounceDelay, uint32_t functionDelay)
    : ButtonBase(buttonType, debounceDelay, functionDelay)
{
    this->pins[0] = pin0;
    this->pins[1] = pin1;
    this->pins[2] = pin2;
    this->pins[3] = pin3;
    this->pins[4] = pin4;
    this->buttonCode = buttonCode;

    pinMode(pin0, INPUT_PULLDOWN);
    pinMode(pin1, INPUT_PULLDOWN);
    pinMode(pin2, INPUT_PULLDOWN);
    pinMode(pin3, INPUT_PULLDOWN);
    pinMode(pin4, INPUT_PULLDOWN);

    this->onPressCb = nullptr;
    this->onLongPressCb = nullptr;
}

void MultiplexedButton::loop()
{
    if(getCurrentButtonCode() == this->buttonCode)
    {
        ButtonBase::press();
    }
    else
    {
        ButtonBase::release();
    }

    ButtonBase::loop();
}

uint8_t MultiplexedButton::getCurrentButtonCode()
{
    byte val = 0x0;
    for(int i=0; i<5; i++)
    {
        if(digitalRead(this->pins[i]) == HIGH)
            val |= (0x1 << i);
    }

    return val;
}

void MultiplexedButton::onPress(void (*func)())
{
    onPressCb = func;
}

void MultiplexedButton::onLongPress(void (*func)())
{
    onLongPressCb = func;
}

void MultiplexedButton::sendPress()
{
    if(onPressCb)
    {
        onPressCb();
    }
}

void MultiplexedButton::sendLongPress()
{
    if(onLongPressCb)
    {
        onLongPressCb();
    }
}
