#ifndef HUDELEMENT_H_
#define HUDELEMENT_H_

#include "QuiteGoodMachine/Source/HUD/Interfaces/HUDElementInterface.h"

class HUDElement : public HUDElementInterface {
   public:
      /**
       * Constructor specifies the initial position
       */
      HUDElement(int x, int y);

      /**
       * Enables the element (allow it to be drawn on screen)
       */
      virtual void Enable();

      /**
       * Disable the element (don't allow it to be drawn on screen)
       */
      virtual void Disable();

      /**
       * Check if element is enabled
       */
      virtual bool IsEnabled();

      /**
       * Makes object interactable
       */
      virtual void MakeInteractable();

      /**
       * Makes object non-interactable
       */
      virtual void MakeNonInteractable();

      /**
       * Checks whether element is able to be interacted with (WIP)
       */
      virtual bool IsInteractable();

   private:
      // Simple drawing enabled/disabled flag
      bool is_enabled_;

      // Simple interactable allowed flag
      bool is_interactable_;
};

#endif