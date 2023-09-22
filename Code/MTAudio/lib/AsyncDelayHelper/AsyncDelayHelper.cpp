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
}

void AsyncDelayHelper::stopDelay()
{
    this->delayFinished = true;
}

bool AsyncDelayHelper::isDelayFinished()
{
    return this->delayFinished;
}

void AsyncDelayHelper::loop()
{
    if(!delayFinished && millis() - startTimestamp >= delay)
    {
        this->delayFinished = true;
    }
}
