//
//  FPSTimer.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/20/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include "QuiteGoodMachine/Source/GameManager/Interfaces/TimerInterface.h"

#include <stdint.h>
#include <ctime>
#include <chrono>

using namespace std::chrono;

class FPSTimer : public TimerInterface<clock_t> {
    public:
        // Initialize
        FPSTimer();
        
        // Clock actions
        virtual void Start();
        virtual void Stop();
        virtual void Pause();
        virtual void Unpause();
        virtual void Reset();
        
        // Get timer time
        virtual clock_t GetTime();

        // Get delta time
        virtual clock_t GetDeltaTime();
        
        // Checks timer status
        virtual bool IsStarted();
        virtual bool IsPaused();
    
    private:
        // The clock start time
        clock_t start_ticks_;
    
        // The ticks sstored when the timer was paused
        clock_t paused_ticks_;

        // Delta ticks
        clock_t delta_;
        clock_t curr_tick_;
        clock_t last_tick_;
    
        // The timer status
        bool is_paused_;
        bool is_started_;
};

#endif /* Timer_h */
