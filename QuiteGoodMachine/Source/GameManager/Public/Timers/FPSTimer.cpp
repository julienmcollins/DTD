#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"

#include <stdio.h>
#include <SDL2/SDL.h>

// Initialize timer
FPSTimer::FPSTimer() : start_ticks_(0), paused_ticks_(0), delta_(0), curr_tick_(0), last_tick_(0),
   is_paused_(false), is_started_(false) {}

// Start timer function
void FPSTimer::Start() {
   if (is_started_) {
      return;
   }

   is_started_ = true;
   is_paused_ = false;
   start_ticks_ = clock();
}

// Stop timer function
void FPSTimer::Stop() {
   is_started_ = false;
}

// Pause timer
void FPSTimer::Pause() {
   if (is_paused_ || !is_started_) {
      return;
   }

   is_paused_ = true;
   paused_ticks_ = clock();
}

// Unpause timer
void FPSTimer::Unpause() {
   if (!is_paused_) {
      return;
   }

   is_paused_ = false;
   start_ticks_ += clock() - paused_ticks_;
}

void FPSTimer::Reset() {
   is_paused_ = false;
   start_ticks_ = clock();
}

// Get tick functions
clock_t FPSTimer::GetTime() {
   if (!is_started_) {
      return 0;
   }

   if (is_paused_) {
      return paused_ticks_ - start_ticks_;
   }

   return clock() - start_ticks_;
}

// Get delta time function
clock_t FPSTimer::GetDeltaTime() {
   curr_tick_ = SDL_GetTicks();
   delta_ = curr_tick_ - last_tick_;
   last_tick_ = curr_tick_;
   return delta_;
}

// Check states
bool FPSTimer::IsStarted() {
    // FPSTimer is running and paused or unpaused
    return is_started_;
}

bool FPSTimer::IsPaused() {
    // FPSTimer is running and paused
    return is_paused_;
}