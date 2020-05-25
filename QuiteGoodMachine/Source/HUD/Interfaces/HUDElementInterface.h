#ifndef HUDELEMENTINTERFACE_H_
#define HUDELEMENTINTERFACE_H_

#include "OpenGLIncludes.h"

class HUDElementInterface {
   public:      
      /**
       * Enables the element (allow it to be drawn on screen)
       */
      virtual void Enable() = 0;

      /**
       * Disable the elemtn (don't allow it to be drawn on screen)
       */
      virtual void Disable() = 0;

      /**
       * Check if element is enabled
       */
      virtual bool IsEnabled() = 0;

      /**
       * Makes object interactable
       */
      virtual void MakeInteractable() = 0;

      /**
       * Makes object non-interactable
       */
      virtual void MakeNonInteractable() = 0;

      /**
       * Checks whether element is able to be interacted with (WIP)
       */
      virtual bool IsInteractable() = 0;

      /**
       * Reset function
       */
      virtual void Reset() = 0;
};

#endif