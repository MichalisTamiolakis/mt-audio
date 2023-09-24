#ifndef SingleButton_h
#define SingleButton_h
#include <ButtonBase.h>
#include <Arduino.h>

class SingleButton : public ButtonBase
{
public:
    SingleButton(uint8_t buttonType, uint8_t pin, uint32_t debounceDelay, uint32_t functionDelay);
    void loop();
    void onPress(void (*func)());
    void onLongPress(void (*func)());

protected:
    void sendLongPress();
    void sendPress();

private:
    uint8_t pin;
    void (*onPressCb)();
    void (*onLongPressCb)();
};

#endif