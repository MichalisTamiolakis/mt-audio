#ifndef FSM_h
#define FSM_h

#include <Arduino.h>
#include <AsyncDelayHelper.h>
#include <System.h>

class FSM
{
public:
    FSM();

    SeekMode seekMode = SeekMode::Auto;
    FMBand fmBand = FMBand::FM1;
    bool isStationSaved = false; // Is the current tuned station saved in the band specified by fmBand?
    
    void loop();
    SystemState getSystemState();
    SystemMode getSystemMode();
    AudioSource getAudioSource();

    void changeSystemState(SystemState newState);
    void changeSystemMode(SystemMode newMode);
    void changeAudioSource(AudioSource newSource);

    void onSystemStateChanged(void (*func)(SystemState newState, SystemState prevState));
    void onSystemModeChanged(void (*func)(SystemMode newMode, SystemMode prevMode));
    void onAudioSourceChanged(void (*func)(AudioSource newSource, AudioSource prevSource));

    void print();
private:
    SystemState previousState;
    SystemState currentState;

    SystemMode previousMode;
    SystemMode currentMode;
    SystemMode modeAfterDelay;

    AudioSource previousSource;
    AudioSource currentSource;

    AsyncDelayHelper delayHelper;
    bool delayStarted = false;

    void (*onSystemStateChangedCb)(SystemState newState, SystemState prevState);
    void (*onSystemModeChangedCb)(SystemMode newMode, SystemMode prevMode);
    void (*onAudioSourceChangedCb)(AudioSource newSource, AudioSource prevSource);

    void startTimeTransition(uint32_t delay, SystemMode nextMode);
    void cancelTransition();
    void onDelayFinished();
};

#endif