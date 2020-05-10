#include "QuiteGoodMachine/Source/GameManager/Private/KeyHandler.h"

#include <iostream>

void KeyHandler::ProcessKeys(const Uint8 *states) {
   states_ = states;
}

bool KeyHandler::GetKeyPressed(KeyMap key) {
   if (states_[key]) {
      return true;
   }
   return false;
}

bool KeyHandler::GetKeyReleased(KeyMap key) {
   if (!states_[key]) {
      return true;
   }
   return false;
}