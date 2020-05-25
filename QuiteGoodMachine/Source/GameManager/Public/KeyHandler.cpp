#include "QuiteGoodMachine/Source/GameManager/Private/KeyHandler.h"

#include <iostream>

KeyHandler::KeyHandler() {
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_LEFT, DoOnce()));
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_RIGHT, DoOnce()));
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_UP, DoOnce()));
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_DOWN, DoOnce()));
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_ENTER, DoOnce()));
   single_presses_.emplace(std::pair<KeyMap, DoOnce>(KEY_SPACE, DoOnce()));
}

void KeyHandler::ProcessKeys(const Uint8 *states) {
   states_ = states;
}

bool KeyHandler::GetKeyPressed(KeyMap key) {
   // Check key state
   if (states_[key]) {
      return true;
   }
   return false;
}

bool KeyHandler::GetKeyReleased(KeyMap key) {
   if (!states_[key]) {
      single_presses_[key].Reset();
      return true;
   }
   return false;
}

bool KeyHandler::GetKeyPressedOnce(KeyMap key) {
   // Check whether key was released
   CheckKeyReleased(key);
   
   // Press key once
   if (states_[key]) {
      bool res = false;
      single_presses_[key]([&]() {
         if (states_[key]) {
            res = true;
         }
      });
      return res;
   }
   return false;
}

bool KeyHandler::GetKeyReleasedOnce(KeyMap key) {
   // if (events_->type == SDL_KEYUP && events_->key.keysym.sym == key) {
   //    return true;
   // }
}

void KeyHandler::CheckKeyReleased(KeyMap key) {
   if (!states_[key]) {
      single_presses_[key].Reset();
   }
}

bool KeyHandler::KeyIsLocked(KeyMap key) {
   single_presses_[key].Locked();
}