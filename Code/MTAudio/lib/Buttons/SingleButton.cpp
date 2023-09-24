#include "SingleButton.h"

SingleButton::SingleButton(uint8_t buttonType, uint8_t pin, uint32_t debounceDelay, uint32_t functionDelay) : 
    ButtonBase(buttonType, debounceDelay, functionDelay)
{
    this->pin = pin;
    pinMode(pin, INPUT_PULLDOWN);
    this->onPressCb = nullptr;
    this->onLongPressCb = nullptr;
}

void SingleButton::loop()
{
    ButtonBase::updateState(digitalRead(pin) == HIGH);
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
