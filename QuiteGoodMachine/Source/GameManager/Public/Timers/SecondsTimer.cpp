#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

#include <stdio.h>
#include <SDL2/SDL.h>

// Initialize timer
SecondsTimer::SecondsTimer() : delta_(0),
   is_paused_(false), is_started_(false) {}

// Start timer function
void SecondsTimer::Start() {
   if (is_started_) {
      return;
   }

   is_started_ = true;
   is_paused_ = false;
   start_ticks_ = steady_clock::now();
}

// Stop timer function
void SecondsTimer::Stop() {
   is_started_ = false;
}

// Pause timer
void SecondsTimer::Pause() {
   if (is_paused_ || !is_started_) {
      return;
   }

   is_paused_ = true;
   paused_ticks_ = steady_clock::now();
}

// Unpause timer
void SecondsTimer::Unpause() {
   if (!is_paused_) {
      return;
   }

   is_paused_ = false;
   start_ticks_ += steady_clock::now() - paused_ticks_;
}

void SecondsTimer::Reset() {
   is_paused_ = false;
   start_ticks_ = steady_clock::now();
}

// Get tick functions
double SecondsTimer::GetTime() {
   if (!is_started_) {
      return 0;
   }

   if (is_paused_) {
      return duration_cast<seconds>(paused_ticks_ - start_ticks_).count();
   }

   return duration_cast<seconds>(steady_clock::now() - start_ticks_).count();
}

// Get delta time function
double SecondsTimer::GetDeltaTime() {
   curr_tick_ = steady_clock::now();
   delta_ = duration_cast<seconds>(curr_tick_ - last_tick_).count();
   last_tick_ = curr_tick_;
   return delta_;
}

// Check states
bool SecondsTimer::IsStarted() {
    // SecondsTimer is running and paused or unpaused
    return is_started_;
}

bool SecondsTimer::IsPaused() {
    // SecondsTimer is running and paused
    return is_paused_;
}