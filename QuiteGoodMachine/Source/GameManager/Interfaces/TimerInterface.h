#ifndef TIMERINTERFACE_H_
#define TIMERINTERFACE_H_

template<class T>
class TimerInterface {
   public:
      /**
       * Start the timer
       */
      virtual void Start() = 0;

      /**
       * Stop the timer
       */
      virtual void Stop() = 0;

      /**
       * Pause the timer
       */
      virtual void Pause() = 0;

      /**
       * Unpause the timer
       */
      virtual void Unpause() = 0;

      /**
       * Reset the timer
       */
      virtual void Reset() = 0;

      /**
       * Get the time
       */
      virtual T GetTime() = 0;

      /**
       * Get the delta time
       */
      virtual T GetDeltaTime() = 0;

      /**
       * Get whether is started
       */
      virtual bool IsStarted() = 0;

      /**
       * Get whether is paused
       */
      virtual bool IsPaused() = 0;
};

#endif