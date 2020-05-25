#ifndef KEYHANDLER_H_
#define KEYHANDLER_H_

#include "QuiteGoodMachine/Source/GameManager/Private/DoOnce.h"

#include <SDL2/SDL.h>

#include <unordered_map>

typedef enum {
   KEY_LEFT = SDL_SCANCODE_LEFT,
   KEY_RIGHT = SDL_SCANCODE_RIGHT,
   KEY_UP = SDL_SCANCODE_UP,
   KEY_DOWN = SDL_SCANCODE_DOWN,

   KEY_ENTER = SDL_SCANCODE_RETURN,
   KEY_SPACE = SDL_SCANCODE_SPACE
} KeyMap;

class KeyHandler {
   public:
      /**
       * Constructor
       */
      KeyHandler();

      /**
       * Process keys
       */
      void ProcessKeys(const Uint8 *states);

      /**
       * Get's whether key is pressed
       */
      bool GetKeyPressed(KeyMap key);

      /**
       * Get's whether key was released
       */
      bool GetKeyReleased(KeyMap key);

      /**
       * Get single key press
       */
      bool GetKeyPressedOnce(KeyMap key);

      /**
       * Get single key released
       */
      bool GetKeyReleasedOnce(KeyMap key);

      /**
       * Check that key is released
       */
      void CheckKeyReleased(KeyMap key);

      /**
       * Key is locked
       */
      bool KeyIsLocked(KeyMap key);
      
   private:
      // Holds key state for registered key
      const Uint8 *states_;

      // Do onces for single key press
      std::unordered_map<KeyMap, DoOnce> single_presses_;
};

#endif