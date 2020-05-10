#ifndef PLAYERSTATES_H_
#define PLAYERSTATES_H_

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

class Texture;
class StateContext;

class PlayerState : public DrawState {
   public:
      /**
       * Constructor
       */
      PlayerState(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform Further action (should flip anims)
       */
      virtual void PerformFurtherAction(StateContext *context);
};

class Player_Stand : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Stand(Texture *texture, std::shared_ptr<Animation> animation1, std::shared_ptr<Animation> animation2, std::shared_ptr<Animation> animation3);

      /**
       * Perform Further action
       */
      virtual void PerformFurtherAction(StateContext *context);

      /**
       * Pre transition
       * From stand to balance if timer goes off
       */
      virtual void PreTransition(StateContext *context);

      /**
       * Post transition function
       * From stand -> left/right goes to running, up goes to jump
       */
      virtual void PostTransition(StateContext *context);
   
   private:
      // Extra animations
      std::shared_ptr<Animation> anim_1_;
      std::shared_ptr<Animation> anim_2_;
      std::shared_ptr<Animation> anim_3_;

      // Timer for balance
      SecondsTimer balance_timer_;
};

class Player_Run : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Run(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_Jump : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Jump(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_Fall : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Fall(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_DoubleJump : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_DoubleJump(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_Push : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Push(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_JumpPush : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_JumpPush(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_Balance : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Balance(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition(StateContext *context);
};

class Player_Death : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Death(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform further action
       */
      virtual void PerformFurtherAction(StateContext *context) {}

      /**
       * Animate function only goes to certain frame
       */
      virtual void Animate();
};

#endif