#include "AsyncDelayHelper.h"
#include <Arduino.h>

AsyncDelayHelper::AsyncDelayHelper()
{
}

void AsyncDelayHelper::startDelay(unsigned long delay)
{
    this->delay = delay;
    this->startTimestamp = millis();
    this->delayFinished = false;
    this->delayFinisedThisTick = false;
}

void AsyncDelayHelper::restartDelay()
{
    this->startTimestamp = millis();
    this->delayFinished = false;
    this->delayFinisedThisTick = false;
}

void AsyncDelayHelper::stopDelay()
{
    this->delayFinished = true;
    this->delayFinisedThisTick = true;
}

bool AsyncDelayHelper::hasDelayFinished()
{
    return this->delayFinished;
}

bool AsyncDelayHelper::hasDelayFinisedThisLoop()
{
    return this->delayFinisedThisTick;
}

void AsyncDelayHelper::loop()
{
    if(delayFinished)
    {
        this->delayFinisedThisTick = false;
        return;
    }
    else if(!delayFinished && millis() - startTimestamp >= delay)
    {
        this->delayFinished = true;
        this->delayFinisedThisTick = true;
    }

}
