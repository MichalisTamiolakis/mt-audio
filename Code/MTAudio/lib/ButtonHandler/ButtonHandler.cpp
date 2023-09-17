#include "ButtonHandler.h"
#include <Arduino.h>

ButtonHandler::ButtonHandler()
{
}

ButtonHandler::ButtonHandler(unsigned long debounceDelay)
{
    this->debounceDelay = debounceDelay;
}

ButtonHandler::ButtonHandler(unsigned long debounceDelay, bool activateOnPress)
{
    this->debounceDelay = debounceDelay;
}

ButtonHandler::ButtonHandler(unsigned long debounceDelay, bool activateOnPress, int minPressDuration)
{
    this->debounceDelay = debounceDelay;
    this->fireEventOnPress = activateOnPress;
    this->minPressDuration = minPressDuration;
}

void ButtonHandler::onPower(void (*func)())
{
    this->onPowerCb = func;
}

void ButtonHandler::onClockSet(void (*func)())
{
    this->onClockSetCb = func;
}

void ButtonHandler::onClockOk(void (*func)())
{
    this->onClockOkCb = func;
}

void ButtonHandler::onVolumeIncrease(void (*func)())
{
    this->onVolumeIncreaseCb = func;
}

void ButtonHandler::onVolumeDecrease(void (*func)())
{
    this->onVolumeDecreaseCb = func;
}

void ButtonHandler::onBasBal(void (*func)())
{
    this->onBasBalCb = func;
}

void ButtonHandler::onTreFad(void (*func)())
{
    this->onTreFadCb = func;
}

void ButtonHandler::onBstLdn(void (*func)())
{
    this->onBstLdnCb = func;
}

void ButtonHandler::onTa(void (*func)())
{
    this->onTaCb = func;
}

void ButtonHandler::onDown(void (*func)())
{
    this->onDownCb = func;
}

void ButtonHandler::onUp(void (*func)())
{
    this->onUpCb = func;
}

void ButtonHandler::onBndMan(void (*func)())
{
    this->onBndManCb = func;
}

void ButtonHandler::onAst(void (*func)())
{
    this->onAstCb = func;
}

void ButtonHandler::on1(void (*func)())
{
    this->on1Cb = func;
}

void ButtonHandler::on2(void (*func)())
{
    this->on2Cb = func;
}

void ButtonHandler::on3(void (*func)())
{
    this->on3Cb = func;
}

void ButtonHandler::on4(void (*func)())
{
    this->on4Cb = func;
}

void ButtonHandler::on5(void (*func)())
{
    this->on5Cb = func;
}

void ButtonHandler::on6(void (*func)())
{
    this->on6Cb = func;
}

void ButtonHandler::init()
{
    // Initialize all inputs as pulldown

    // Single buttons
    pinMode(PWR_BTN, INPUT_PULLDOWN);
    pinMode(CLK_BTN_SET, INPUT_PULLDOWN);
    pinMode(CLK_BTN_OK, INPUT_PULLDOWN);

    // Multiplexed buttons
    pinMode(BTN_B0, INPUT_PULLDOWN);
    pinMode(BTN_B1, INPUT_PULLDOWN);
    pinMode(BTN_B2, INPUT_PULLDOWN);
    pinMode(BTN_B3, INPUT_PULLDOWN);
    pinMode(BTN_B4, INPUT_PULLDOWN);
}

void ButtonHandler::update()
{
    checkPress();
    checkRelease();

    checkCallbackFire();
}

void ButtonHandler::checkPress()
{
    unsigned long currMillis = millis();
    // Check single
    if (!this->powerPrevState && digitalRead(PWR_BTN) == HIGH && currMillis - this->powerStateChangedTimestamp >= debounceDelay)
    {
        this->powerPrevState = true;
        this->powerStateChangedTimestamp = currMillis;
        this->powerEventFired = false;
    }
    if (!this->clockOkPrevState && digitalRead(CLK_BTN_OK) == HIGH && currMillis - this->clockOkStateChangedTimestamp >= debounceDelay)
    {
        this->clockOkPrevState = true;
        this->clockOkStateChangedTimestamp = currMillis;
        this->clockOkEventFired = false;
    }
    if (!this->clockSetPrevState && digitalRead(CLK_BTN_SET) == HIGH && currMillis - this->clockSetStateChangedTimestamp >= debounceDelay)
    {
        this->clockSetPrevState = true;
        this->clockSetStateChangedTimestamp = currMillis;
        this->clockSetEventFired = false;
    }

    // Check multiplexed buttons
    byte currButtonsValue = readMultiplexedButtons();
    if (currButtonsValue != 0x0 && currButtonsValue != buttonsPrevValue && currMillis - this->buttonsValueChangedTimestamp >= debounceDelay)
    {
        buttonsValueChangedTimestamp = millis();
        buttonsPrevValue = currButtonsValue;
        buttonsEventFired = false;
    }
}

void ButtonHandler::checkRelease()
{
    unsigned long currMillis = millis();
    // Check single
    if (this->powerPrevState && digitalRead(PWR_BTN) == LOW && currMillis - this->powerStateChangedTimestamp >= debounceDelay)
    {
        this->powerPrevState = false;
        this->powerStateChangedTimestamp = currMillis;
        this->powerEventFired = false;
    }
    if (this->clockOkPrevState && digitalRead(CLK_BTN_OK) == LOW && currMillis - this->clockOkStateChangedTimestamp >= debounceDelay)
    {
        this->clockOkPrevState = false;
        this->clockOkStateChangedTimestamp = currMillis;
        this->clockOkEventFired = false;
    }
    if (this->clockSetPrevState && digitalRead(CLK_BTN_SET) == LOW && currMillis - this->clockSetStateChangedTimestamp >= debounceDelay)
    {
        this->clockSetPrevState = false;
        this->clockSetStateChangedTimestamp = currMillis;
        this->clockSetEventFired = false;
    }

    // Check multiplexed buttons
    byte currButtonsValue = readMultiplexedButtons();
    if (currButtonsValue == 0x0 && currButtonsValue != buttonsPrevValue && currMillis - this->buttonsValueChangedTimestamp >= debounceDelay)
    {
        buttonsValueChangedTimestamp = millis();
        buttonsPrevValue = currButtonsValue;
        this->buttonsEventFired = false;
    }
}

void ButtonHandler::checkCallbackFire()
{
    unsigned long currMillis = millis();
    // Check for onPressed Events
    if (fireEventOnPress)
    {
        // Check single
        if (!this->powerEventFired && digitalRead(PWR_BTN) == HIGH && currMillis - powerStateChangedTimestamp >= minPressDuration)
        {
            this->powerEventFired = true;
            this->onPowerCb();
        }
        if (!this->clockOkEventFired && digitalRead(CLK_BTN_OK) == HIGH && currMillis - clockOkStateChangedTimestamp >= minPressDuration)
        {
            this->clockOkEventFired = true;
            this->onClockOkCb();
        }
        if (!this->clockSetEventFired && digitalRead(CLK_BTN_SET) == HIGH && currMillis - clockSetStateChangedTimestamp >= minPressDuration)
        {
            this->clockSetEventFired = true;
            this->onClockSetCb();
        }

        // Check multiplexed buttons
        char currButtonsValue = readMultiplexedButtons();
        if (!this->buttonsEventFired && currButtonsValue != 0x0 && currMillis - this->buttonsValueChangedTimestamp >= minPressDuration)
        {
            this->buttonsEventFired = true;
            this->callMultiplexedButtonCb(currButtonsValue);
        }
    }
    // Check for onReleased Events
    else
    {
        // Check single
        if (!this->powerEventFired && digitalRead(PWR_BTN) == LOW && currMillis - powerStateChangedTimestamp >= minPressDuration)
        {
            this->powerEventFired = true;
            if (this->onPowerCb)
                this->onPowerCb();
        }
        if (!this->clockOkEventFired && digitalRead(CLK_BTN_OK) == LOW && currMillis - clockOkStateChangedTimestamp >= minPressDuration)
        {
            this->clockOkEventFired = true;
            if (this->onClockOkCb)
                this->onClockOkCb();
        }
        if (!this->clockSetEventFired && digitalRead(CLK_BTN_SET) == LOW && currMillis - clockSetStateChangedTimestamp >= minPressDuration)
        {
            this->clockSetEventFired = true;
            if (this->onClockSetCb)
                this->onClockSetCb();
        }

        // TODO somehow store the previous value somewhere.
        // Check multiplexed buttons
        byte currButtonsValue = readMultiplexedButtons();
        if (!this->buttonsEventFired && currButtonsValue != 0x0 && currMillis - this->buttonsValueChangedTimestamp >= minPressDuration)
        {
            this->buttonsEventFired = true;
            this->callMultiplexedButtonCb(currButtonsValue);
        }
    }
}

byte ButtonHandler::readMultiplexedButtons()
{
    byte val = 0x0;
    if (digitalRead(BTN_B0))
        val |= 1;
    if (digitalRead(BTN_B1))
        val |= (0x1 << 1);
    if (digitalRead(BTN_B2))
        val |= (0x1 << 2);
    if (digitalRead(BTN_B3))
        val |= (0x1 << 3);
    if (digitalRead(BTN_B4))
        val |= (0x1 << 4);

    return val;
}

void ButtonHandler::callMultiplexedButtonCb(byte btn)
{
    switch (btn)
    {
    case VOLUME_INCREASE:
        if (this->onVolumeIncreaseCb)
            this->onVolumeIncreaseCb();
        break;
    case VOLUME_DECREASE:
        if (this->onVolumeDecreaseCb)
            this->onVolumeDecreaseCb();
        break;
    case BAS_BAL:
        if (this->onBasBalCb)
            this->onBasBalCb();
        break;
    case TRE_FAD:
        if (this->onTreFadCb)
            this->onTreFadCb();
        break;
    case BST_LDN:
        if (this->onBstLdnCb)
            this->onBstLdnCb();
        break;
    case TA:
        if (this->onTaCb)
            this->onTaCb();
        break;
    case DOWN:
        if (this->onDownCb)
            this->onDownCb();
        break;
    case UP:
        if (this->onUpCb)
            this->onUpCb();
        break;
    case BND_MAN:
        if (this->onBndManCb)
            this->onBndManCb();
        break;
    case AST:
        if (this->onAstCb)
            this->onAstCb();
        break;
    case N1:
        if (this->on1Cb)
            this->on1Cb();
        break;
    case N2:
        if (this->on2Cb)
            this->on2Cb();
        break;
    case N3:
        if (this->on3Cb)
            this->on3Cb();
        break;
    case N4:
        if (this->on4Cb)
            this->on4Cb();
        break;
    case N5:
        if (this->on5Cb)
            this->on5Cb();
        break;
    case N6:
        if (this->on6Cb)
            this->on6Cb();
        break;
    }
}