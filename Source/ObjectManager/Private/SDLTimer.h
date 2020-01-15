#ifndef SDLTIMER_H_
#define SDLTIMER_H

#include <stdint.h>

class SDLTimer {
   public:
      //Initializes variables
      SDLTimer();

      //The various clock actions
      void Start();
      void Stop();
      void Pause();
      void Unpause();

      //Gets the timer's time
      unsigned int GetTicks();

      // Get delta time
      int getDeltaTime();

      //Checks the status of the timer
      bool isStarted();
      bool isPaused();

   private:
      //The clock time when the timer started
      unsigned int mStartTicks;

      //The ticks stored when the timer was paused
      unsigned int mPausedTicks;

      // Delta ticks
      int delta_;
      int curr_tick_;
      int last_tick_;
      
      //The timer status
      bool mPaused;
      bool mStarted;
};

#endif