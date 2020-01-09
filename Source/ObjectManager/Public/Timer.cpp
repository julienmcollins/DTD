//
//  Timer.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/20/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include "Source/ObjectManager/Private/Timer.h"

#include <stdio.h>
#include <SDL2/SDL.h>

// Initialize timer
Timer::Timer() : start_ticks_(0), paused_ticks_(0), delta_(0), curr_tick_(0), last_tick_(0),
   is_paused_(false), is_started_(false) {}

// Start timer function
void Timer::Start() {
   if (is_started_) {
      return;
   }

   is_started_ = true;
   is_paused_ = false;
   start_ticks_ = clock();
}

// Stop timer function
void Timer::Stop() {
   is_started_ = false;
}

// Pause timer
void Timer::Pause() {
   if (is_paused_ || !is_started_) {
      return;
   }

   is_paused_ = true;
   paused_ticks_ = clock();
}

// Unpause timer
void Timer::Unpause() {
   if (!is_paused_) {
      return;
   }

   is_paused_ = false;
   start_ticks_ += clock() - paused_ticks_;
}

void Timer::Reset() {
   is_paused_ = false;
   start_ticks_ = clock();
}

// Get tick functions
clock_t Timer::GetTicks() {
   if (!is_started_) {
      return 0;
   }

   if (is_paused_) {
      return paused_ticks_ - start_ticks_;
   }

   return clock() - start_ticks_;
}

// Get delta time function
int Timer::getDeltaTime() {
   curr_tick_ = SDL_GetTicks();
   delta_ = curr_tick_ - last_tick_;
   last_tick_ = curr_tick_;
   return delta_;
}

// Check states
bool Timer::isStarted() {
    // Timer is running and paused or unpaused
    return is_started_;
}

bool Timer::isStopped() {
   return !is_started_;
}

bool Timer::isPaused() {
    // Timer is running and paused
    return is_paused_;
}

bool Timer::isActive() {
   return !is_paused_ && is_started_;
}
