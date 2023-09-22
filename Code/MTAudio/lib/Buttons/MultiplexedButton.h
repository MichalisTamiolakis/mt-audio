#ifndef MultiplexedButton_h
#define MultiplexedButton_h
#include <ButtonBase.h>
#include <Arduino.h>

class MultiplexedButton : public ButtonBase
{
public:
    MultiplexedButton(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t buttonCode, uint32_t debounceDelay, uint32_t longPressDelay, bool allowRepeat);
    void loop();
    uint8_t getCurrentButtonCode();
    void onPress(void (*func)());
    void onLongPress(void (*func)());

protected:
    void sendPress();
    void sendLongPress();

private:
    uint8_t pins[5];
    uint8_t buttonCode;
    void (*onPressCb)();
    void (*onLongPressCb)();
};

#endif