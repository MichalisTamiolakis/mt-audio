#ifndef AsyncDelayHelper_h
#define AsyncDelayHelper_h

class AsyncDelayHelper{
    public:
        AsyncDelayHelper();
        
        void startDelay(unsigned long delay); // Add delayed call. If previous already exists it will be stopped.
        void stopDelay(); // Force stop delayed call
        bool isDelayFinished(); // Check if delayed call is finished

        void loop(); // Call every tick to check for pending calls

    private:
        unsigned long delay;
        unsigned long startTimestamp;
        bool delayFinished = false;

};
#endif