//
//  Timer.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/20/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include <stdint.h>
#include <ctime>

class Timer {
    public:
        // Initialize
        Timer();
        
        // Clock actions
        void Start();
        void Stop();
        void Pause();
        void Unpause();
        void Reset();
        
        // Get timer time
        clock_t GetTicks();

        // Get delta time
        int getDeltaTime();
        
        // Checks timer status
        bool isStarted();
        bool isStopped();
        bool isPaused();
        bool isActive();
    
    private:
        // The clock start time
        clock_t start_ticks_;
    
        // The ticks sstored when the timer was paused
        clock_t paused_ticks_;

        // Delta ticks
        int delta_;
        int curr_tick_;
        int last_tick_;
    
        // The timer status
        bool is_paused_;
        bool is_started_;
};

#endif /* Timer_h */
