#include "FSM.h"

FSM::FSM()
{
    this->delayHelper = AsyncDelayHelper();

    this->previousState = SystemState::Off;
    this->previousMode = SystemMode::Idle;
    
    this->currentMode = SystemMode::Idle;
    this->currentState = SystemState::Off;
    
    this->previousSource = AudioSource::FM1;
    this->currentSource = AudioSource::FM1;

    this->onSystemStateChangedCb = nullptr;
    this->onSystemModeChangedCb = nullptr;
    this->onAudioSourceChangedCb = nullptr;
}

void FSM::loop()
{
    delayHelper.loop();
    if(delayStarted && delayHelper.isDelayFinished())
    {
        delayStarted = false;
        onDelayFinished();
    }
}

SystemState FSM::getSystemState()
{
    return this->currentState;
}

SystemMode FSM::getSystemMode()
{
    return this->currentMode;
}

AudioSource FSM::getAudioSource()
{
    return this->currentSource;
}

void FSM::changeSystemState(SystemState newState)
{
    if(newState != currentState)
    {
        previousState = currentState;
        currentState = newState;

        if (onSystemStateChangedCb)
        {
            onSystemStateChangedCb(currentState, previousState);
        }
    }
}

void FSM::changeSystemMode(SystemMode newMode)
{

    if(newMode != currentMode)
    {
        previousMode = currentMode;
        currentMode = newMode;

        if (onSystemModeChangedCb)
        {
            onSystemModeChangedCb(currentMode, previousMode);
        }
    }

    // For states that have a timer transition, start the time
    switch (currentMode)
    {
        case SystemMode::TurnOnSequence:
            startTimeTransition(5000, SystemMode::Idle);
            break;
        case SystemMode::Volume:
            startTimeTransition(2000, SystemMode::Idle);
            break;
    }
}

void FSM::changeAudioSource(AudioSource newSource)
{
    if(newSource != currentSource)
    {
        previousSource = currentSource;
        currentSource = newSource;

        if (onAudioSourceChangedCb)
        {
            onAudioSourceChangedCb(currentSource, previousSource);
        }
    }
}

void FSM::onSystemStateChanged(void (*func)(SystemState newState, SystemState prevState))
{
    this->onSystemStateChangedCb = func;
}

void FSM::onSystemModeChanged(void (*func)(SystemMode newMode, SystemMode prevMode))
{
    this->onSystemModeChangedCb = func;
}

void FSM::onAudioSourceChanged(void (*func)(AudioSource newSource, AudioSource prevSource))
{
    this->onAudioSourceChangedCb = func;
}

void FSM::startTimeTransition(uint32_t delay, SystemMode nextMode)
{
    delayHelper.startDelay(delay);
    delayStarted = true;
    modeAfterDelay = nextMode;
}

void FSM::cancelTransition()
{
    delayHelper.stopDelay();
    delayStarted = false;
}

void FSM::onDelayFinished()
{
    changeSystemMode(modeAfterDelay);
}

void FSM::print()
{
    Serial.print("[FSM] S[");
    switch (this->currentState)
    {
        case SystemState::Off:
            Serial.print("Off");
            break;
        case SystemState::Standby:
            Serial.print("Standby");
            break;
        case SystemState::On:
            Serial.print("On");
            break;
        default:
            Serial.print("Unknown");
            break;
    }
    Serial.print("], M[");
    switch (this->currentMode)
    {
        case SystemMode::TurnOnSequence:
            Serial.print("TurnOnSequence");
            break;
        case SystemMode::Idle:
            Serial.print("Idle");
            break;
        case SystemMode::InputSelection:
            Serial.print("InputSelection");
            break;
        case SystemMode::Volume:
            Serial.print("Volume");
            break;
        case SystemMode::Bass:
            Serial.print("Bass");
            break;
        case SystemMode::Treble:
            Serial.print("Treble");
            break;
        case SystemMode::Balance:
            Serial.print("Balance");
            break;
        case SystemMode::Fade:
            Serial.print("Fade");
            break;
        case SystemMode::Loudness:
            Serial.print("Loudness");
            break;
        case SystemMode::BstLdn:
            Serial.print("BstLdn");
            break;
        case SystemMode::StationSave:
            Serial.print("StationSave");
            break;
        case SystemMode::SeekModeSet:
            Serial.print("SeekModeSet");
            break;
        case SystemMode::Brightness:
            Serial.print("Brightness");
            break;
        case SystemMode::MinutesSet:
            Serial.print("MinutesSet");
            break;
        case SystemMode::HoursSet:
            Serial.print("HoursSet");
            break;
        case SystemMode::YearsSet:
            Serial.print("YearsSet");
            break;
        case SystemMode::MonthsSet:
            Serial.print("MonthsSet");
            break;
        case SystemMode::DaysSet:
            Serial.print("DaysSet");
            break;
        case SystemMode::ShowDate:
            Serial.print("ShowDate");
            break;
        default:
            Serial.print("Unknown");
            break;
    }
    Serial.print("], A[");
    switch(this->currentSource)
    {
        case AudioSource::FM1:
            Serial.print("FM1");
            break;
        case AudioSource::FM2:
            Serial.print("FM2");
            break;
        case AudioSource::FMBst:
            Serial.print("FMBst");
            break;
        case AudioSource::Aux:
            Serial.print("Aux");
            break;
        case AudioSource::Bluetooth:
            Serial.print("Bluetooth");
            break;
        case AudioSource::USB:
            Serial.print("USB");
            break;
        case AudioSource::SD:
            Serial.print("SD");
            break;
        default:
            Serial.print("Unknown");
            break;
    }
    Serial.println("]");
}