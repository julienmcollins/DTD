#ifndef SECONDSTIMER_H_
#define SECONDSTIMER_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/TimerInterface.h"

#include <chrono>

using namespace std::chrono;

class SecondsTimer : public TimerInterface<double> {
    public:
        // Initialize
        SecondsTimer();
        
        // Clock actions
        virtual void Start();
        virtual void Stop();
        virtual void Pause();
        virtual void Unpause();
        virtual void Reset();
        
        // Get timer time
        virtual double GetTime();

        // Get delta time
        virtual double GetDeltaTime();
        
        // Checks timer status
        virtual bool IsStarted();
        virtual bool IsPaused();
    
    private:
        // The clock start time
        steady_clock::time_point start_ticks_;
    
        // The ticks sstored when the timer was paused
        steady_clock::time_point paused_ticks_;

        // Delta ticks
        double delta_;
        steady_clock::time_point curr_tick_;
        steady_clock::time_point last_tick_;
    
        // The timer status
        bool is_paused_;
        bool is_started_;
};

#endif