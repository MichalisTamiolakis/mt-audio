#ifndef ButtonHandler_h
#define ButtonHandler_h
#include <Arduino.h>

// Button I/O pins
#define PWR_BTN  33
#define CLK_BTN_SET 25
#define CLK_BTN_OK 23
#define BTN_B0 13
#define BTN_B1 12
#define BTN_B2 14
#define BTN_B3 27
#define BTN_B4 26

// Multiplexed button codes
#define VOLUME_INCREASE 0x12
#define VOLUME_DECREASE 0x18
#define BAS_BAL 0x2
#define TRE_FAD 0x10
#define BST_LDN 0xC
#define TA 0x9
#define DOWN 0x5
#define UP 0x14
#define BND_MAN 0x4
#define AST 0x7
#define N1 0x11
#define N2 0xA
#define N3 0x6
#define N4 0x3
#define N5 0x8
#define N6 0x1

class ButtonHandler{
    public:
        ButtonHandler();
        ButtonHandler(unsigned long debounceDelay);
        ButtonHandler(unsigned long debounceDelay, bool activateOnPress);
        ButtonHandler(unsigned long debounceDelay, bool activateOnPress, int minPressDuration);
        
        // Multiplexed buttons
        void onVolumeIncrease(void (*func)()); // Increase volume button (and other stuff in special cases) 
        void onVolumeDecrease(void (*func)()); // Decrease volume button (and other stuff in special cases)
        void onBasBal(void (*func)()); // Button performing Bass/Balance function
        void onTreFad(void (*func)()); // Button performing Treble/Fade function
        void onBstLdn(void (*func)()); // Best/
        void onTa(void (*func)()); // Traffic announcements
        void onDown(void (*func)()); // Tune down
        void onUp(void (*func)()); // Tune up
        void onBndMan(void (*func)()); // Band change
        void onAst(void (*func)()); // Auto station
        void on1(void (*func)()); // num 1
        void on2(void (*func)()); // num 2
        void on3(void (*func)()); // num 3
        void on4(void (*func)()); // num 4
        void on5(void (*func)()); // num 5
        void on6(void (*func)()); // num 6
        
        // Single buttons
        void onPower(void (*func)());
        void onClockSet(void (*func)());
        void onClockOk(void (*func)());

        void init(); // Call on init to initialize I/O pins

        void update(); // Call every clock cycle to poll for button presses/releases
    
    private:
        unsigned long debounceDelay = 10;
        unsigned long minPressDuration = 10;

        byte buttonsPrevValue = 0x0;
        bool buttonsEventFired = false;
        bool powerPrevState = false;
        bool powerEventFired = false;
        bool clockSetPrevState = false;
        bool clockSetEventFired = false;
        bool clockOkPrevState = false;
        bool clockOkEventFired = false;
        bool fireEventOnPress = true; // True for onPress, false for onReleased.

        unsigned long buttonsValueChangedTimestamp;
        unsigned long powerStateChangedTimestamp;
        unsigned long clockSetStateChangedTimestamp;
        unsigned long clockOkStateChangedTimestamp;

        void (*onPowerCb)();
        void (*onClockSetCb)();
        void (*onClockOkCb)();

        // Function callbacks
        void (*onVolumeIncreaseCb)();
        void (*onVolumeDecreaseCb)();
        void (*onBasBalCb)();
        void (*onTreFadCb)();
        void (*onBstLdnCb)();
        void (*onTaCb)();
        void (*onDownCb)();
        void (*onUpCb)();
        void (*onBndManCb)();
        void (*onAstCb)();
        void (*on1Cb)();
        void (*on2Cb)();
        void (*on3Cb)();
        void (*on4Cb)();
        void (*on5Cb)();
        void (*on6Cb)();

        void checkPress();
        void checkRelease();
        void checkCallbackFire();
        byte readMultiplexedButtons();
        void callMultiplexedButtonCb(byte btn);
};

#endif

