#include "ButtonBase.h"

ButtonBase::ButtonBase(uint32_t debounceDelay, uint32_t longPressDelay, bool allowRepeat)
{
    this->debounce_delay = debounceDelay;
    this->repetitive = allowRepeat;
    this->longPressDelay = longPressDelay;
}

void ButtonBase::press()
{
    this->state = true;
}

void ButtonBase::release()
{
    this->state = false;
}

void ButtonBase::updateState(bool newState)
{
    this->state = newState;
}

void ButtonBase::loop()
{
    // State changed
    if(previousState!=state)
    {
        stateChangeTimestamp = millis();
        repetitionStartTime = millis();
        firstPressEventTriggered = false;
        long_press_event_triggered = false;
        previousState = state;
    }

    uint32_t currentTime = millis();
    if(state && currentTime-stateChangeTimestamp >= debounce_delay)
    {
        if(!repetitive)
        {
            if(!firstPressEventTriggered)
            {
                firstPressEventTriggered = true;

                // Call on press
                sendPress();
            }

            if(!long_press_event_triggered && currentTime - stateChangeTimestamp >= longPressDelay)
            {
                long_press_event_triggered = true;

                // Call on long press
                sendLongPress();
            }
        }
        else
        {
            if(!firstPressEventTriggered)
            {
                firstPressEventTriggered = true;

                // Call on press
                sendPress();
            }
            else if(currentTime - repetitionStartTime >= longPressDelay)
            {
                repetitionStartTime = currentTime;

                // Call on press
                sendPress();
            }
        }
    }
}