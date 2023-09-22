#include "SingleButton.h"

SingleButton::SingleButton(uint8_t pin, uint32_t debounceDelay, uint32_t longPressDelay, bool allowRepeat) : 
    ButtonBase(debounceDelay, longPressDelay, allowRepeat)
{
    this->pin = pin;
    pinMode(pin, INPUT_PULLDOWN);
    this->onPressCb = nullptr;
    this->onLongPressCb = nullptr;
}

void SingleButton::loop()
{
    ButtonBase::updateState(digitalRead(pin));
    ButtonBase::loop();
}

void SingleButton::onPress(void (*func)())
{
    onPressCb = func;
}

void SingleButton::onLongPress(void (*func)())
{
    onLongPressCb = func;
}

void SingleButton::sendPress()
{
    if(onPressCb)
    {
        onPressCb();
    }
}

void SingleButton::sendLongPress()
{
    if(onLongPressCb)
    {
        onLongPressCb();
    }
}
