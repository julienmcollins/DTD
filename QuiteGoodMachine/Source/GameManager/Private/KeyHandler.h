#ifndef KEYHANDLER_H_
#define KEYHANDLER_H_

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

   private:
      // Holds key state for registered key
      const Uint8 *states_;
};

#endif