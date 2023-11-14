#ifndef ENUMS_H
#define ENUMS_H
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
    Radio, // Radio
    Aux, // Aux
    Bluetooth, // Bluetooth
    USB, // USB
    SD // SD
};

enum class RadioSeekMode
{
    Auto, // Up and down buttons go to the next good received station
    Manual // Up and down buttons go to the next station
};

enum class FMBand
{
    FM = -1, // FM band generic not saved
    FM1 = 0, // Saved stations in fm1
    FM2 = 1, // Saved stations in fm2
    FMBst = 2  // Saved stations after AST search.
};

#endif