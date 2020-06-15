#ifndef DRAWSTATE_H_
#define DRAWSTATE_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include <memory>
#include <string>

class DrawStateContext;

class DrawState : public StateInterface {
   public:
      /**
       * Constructor takes an animation
       */
      DrawState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform action --> gets decomposed further
       */
      virtual void PerformAction();

      /**
       * Perform further action --> name better?
       */
      virtual void PerformFurtherAction();

      /**
       * Set state function for short hand
       */
      void SetState(std::string name);

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
      virtual void PreTransition();

   private:
      // Texture for animation handle
      Texture *texture_;

      // Animation handle
      std::shared_ptr<Animation> animation_;

      // Draw state context
      DrawStateContext *draw_context_;

   public:
      /**
       * Get draw state context
       */
      DrawStateContext *GetContext() {
         return draw_context_;
      }
};

#endif