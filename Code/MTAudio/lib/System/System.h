#ifndef System_H
#define System_H
enum class SystemState
{
    Off,
    Standby,
    On
};

enum class SystemMode
{
    TurnOnSequence,
    Idle,

    InputSelection,
    Volume,
    Bass,
    Treble,
    Balance,
    Fade,
    Loudness,

    BstLdn,
    StationSave,
    SeekModeSet,
    
    Brightness,
    
    MinutesSet,
    HoursSet,
    YearsSet,
    MonthsSet,
    DaysSet,

    ShowDate,
};

enum class AudioSource
{
    FM1, // Saved stations in fm1
    FM2, // Saved stations in fm2
    FMBst, // Best stations
    Aux, // Aux
    Bluetooth, // Bluetooth
    USB, // USB
    SD // SD
};
#endif