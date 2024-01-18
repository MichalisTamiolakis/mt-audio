#ifndef ButtonBase_h
#define ButtonBase_h
#include <Arduino.h>

#define BUTTON_SINGLE 0U // Button with single press
#define BUTTON_LONG 1U   // Button with single press and long press
#define BUTTON_REPEAT 2U // Button with single press and repeated press

class ButtonBase
{
public:
    ButtonBase(uint8_t buttonType, uint32_t debounceDelay, uint32_t functionDelay);

    virtual void loop();

protected:
    uint8_t buttonType;

    uint32_t longPressDelay = 0;
    uint32_t debounceDelay = 100;

    bool state = false;
    bool previousState = false;

    bool debouncedState = false;
    bool previousDebouncedState = false;

    uint32_t stateChangeTimestamp = 0;
    uint32_t pressedTime = 0;

    bool longPressSent = false;

    virtual void press();
    virtual void release();
    virtual void updateState(bool newState);
    virtual void sendPress() = 0;
    virtual void sendLongPress() = 0;

private:
};

#endif