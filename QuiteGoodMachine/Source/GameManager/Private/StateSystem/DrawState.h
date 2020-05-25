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
      DrawState(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform action --> gets decomposed further
       */
      virtual void PerformAction();

      /**
       * Perform further action --> name better?
       */
      virtual void PerformFurtherAction();

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

      /**
       * Main draw function
       */
      virtual void Draw();

      /**
       * Flip function
       */
      virtual void Turn();

      /**
       * Reset function
       */
      virtual void Reset();

      /**
       * Transition reset function for during state change
       */
      virtual void TransitionReset();

   private:
      // Texture for animation handle
      Texture *texture_;

      // Animation handle
      std::shared_ptr<Animation> animation_;
};

#endif