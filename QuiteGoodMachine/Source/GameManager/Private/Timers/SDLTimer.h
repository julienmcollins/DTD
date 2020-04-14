#ifndef SDLTIMER_H_
#define SDLTIMER_H

#include "QuiteGoodMachine/Source/GameManager/Interfaces/TimerInterface.h"

#include <stdint.h>

class SDLTimer : public TimerInterface<unsigned int> {
   public:
      //Initializes variables
      SDLTimer();

      //The various clock actions
      virtual void Start();
      virtual void Stop();
      virtual void Pause();
      virtual void Unpause();
      virtual void Reset();

      //Gets the timer's time
      virtual unsigned int GetTime();

      // Get delta time
      virtual unsigned int GetDeltaTime();

      //Checks the status of the timer
      virtual bool IsStarted();
      virtual bool IsPaused();

   private:
      //The clock time when the timer started
      unsigned int mStartTicks;

      //The ticks stored when the timer was paused
      unsigned int mPausedTicks;

      // Delta ticks
      unsigned int delta_;
      unsigned int curr_tick_;
      unsigned int last_tick_;
      
      //The timer status
      bool mPaused;
      bool mStarted;
};

#endif