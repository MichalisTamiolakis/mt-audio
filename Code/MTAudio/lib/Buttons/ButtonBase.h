#ifndef ButtonBase_h
#define ButtonBase_h
#include <Arduino.h>

class ButtonBase
{
public:
    ButtonBase(uint32_t debounce_delay, uint32_t long_press_delay, bool allow_repeat);

    virtual void loop();

protected:
    bool repetitive = false; // If enabled the button will call the press event every long_press millis. The LongPress event will not be called.
    uint32_t longPressDelay = 0;
    uint32_t debounce_delay = 100;

    bool state = false;
    bool previousState = false;
    uint32_t stateChangeTimestamp = 0;
    uint32_t repetitionStartTime = 0;
    bool firstPressEventTriggered = false;
    bool long_press_event_triggered = false;

    virtual void press();
    virtual void release();
    virtual void updateState(bool newState);
    virtual void sendPress() = 0;
    virtual void sendLongPress() = 0;

private:
};

#endif