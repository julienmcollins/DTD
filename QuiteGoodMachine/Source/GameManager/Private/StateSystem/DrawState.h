#ifndef DRAWSTATE_H_
#define DRAWSTATE_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include <memory>

class DrawState : public StateInterface {
   public:
      /**
       * Constructor takes an animation
       */
      DrawState(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform action --> gets decomposed further
       */
      virtual void PerformAction(StateContext *context);

      /**
       * Perform further action --> name better?
       */
      virtual void PerformFurtherAction(StateContext *context);

      /**
       * Get texture handle
       */
      Texture *GetTexture();

      /**
       * Get animation
       */
      std::shared_ptr<Animation> GetAnimation();

      /**
       * Set animation
       */
      void SetAnimation(std::shared_ptr<Animation> anim);

      /**
       * Animate the animation
       */
      virtual void Animate();

   private:
      // Texture for animation handle
      Texture *texture_;

      // Animation handle
      std::shared_ptr<Animation> animation_;
};

#endif