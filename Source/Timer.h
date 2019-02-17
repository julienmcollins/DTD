//
//  Timer.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/20/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

class Timer {
    public:
        // Initialize
        Timer();
        
        // Clock actions
        void start();
        void stop();
        void pause();
        void unpause();
        
        // Get timer time
        uint32_t getTicks();

        // Get delta time
        int getDeltaTime();
        
        // Checks timer status
        bool isStarted();
        bool isPaused();
    
    private:
        // The clock start time
        uint32_t m_startTicks;
    
        // The ticks sstored when the timer was paused
        uint32_t m_pausedTicks;

        // Delta ticks
        int delta_;
        int curr_tick_;
        int last_tick_;
    
        // The timer status
        bool m_paused;
        bool m_started;
};

#endif /* Timer_h */
