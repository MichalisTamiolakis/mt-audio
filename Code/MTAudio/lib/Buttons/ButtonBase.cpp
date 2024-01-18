#include "ButtonBase.h"

ButtonBase::ButtonBase(uint8_t buttonType, uint32_t debounceDelay, uint32_t functionDelay)
{
    this->buttonType = buttonType;
    this->debounceDelay = debounceDelay;
    this->longPressDelay = functionDelay;
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
    uint32_t currentTime = millis();

    // State changed
    if (previousState != state)
    {
        stateChangeTimestamp = currentTime;
    }
    previousState = state;

    // Debounce logic
    if (currentTime - stateChangeTimestamp > debounceDelay)
    {
        previousDebouncedState = debouncedState;
        debouncedState = state;

        // Pressed at this moment
        if (debouncedState && !previousDebouncedState)
        {
            switch (this->buttonType)
            {
            case BUTTON_REPEAT:
            case BUTTON_SINGLE:
                sendPress();
                break;
            case BUTTON_LONG:
                longPressSent = false;
                break;
            }

            pressedTime = currentTime;
        }

        // Released at this moment
        else if (!debouncedState && previousDebouncedState)
        {
            switch (this->buttonType)
            {
            // Check if should send the single press event
            case BUTTON_LONG:
                if (currentTime - pressedTime < longPressDelay)
                {
                    sendPress();
                }
                break;
            }
        }

        if (debouncedState)
        {
            switch (this->buttonType)
            {
            case BUTTON_REPEAT:
                if (currentTime - pressedTime >= longPressDelay)
                {
                    sendPress();
                    pressedTime = currentTime;
                }
                break;
            case BUTTON_LONG:
                if (currentTime - pressedTime >= longPressDelay && !longPressSent)
                {
                    longPressSent = true;
                    sendLongPress();
                }
                break;
            }
        }
    }
}