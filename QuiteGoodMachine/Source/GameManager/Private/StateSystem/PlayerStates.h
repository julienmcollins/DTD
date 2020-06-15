#ifndef PLAYERSTATES_H_
#define PLAYERSTATES_H_

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

#include "QuiteGoodMachine/Source/GameManager/Private/DoOnce.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

class Texture;
class DrawStateContext;
class Player;

class PlayerState : public DrawState {
   public:
      /**
       * Constructor
       */
      PlayerState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Turn function flips
       */
      virtual void Turn();
   
   protected:
      // Player pointer
      Player *player;
};

class Player_Stand : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Stand(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation1, std::shared_ptr<Animation> animation2, std::shared_ptr<Animation> animation3);

      /**
       * Perform Further action
       */
      virtual void PerformFurtherAction();

      /**
       * Pre transition
       * From stand to balance if timer goes off
       */
      virtual void PreTransition();

      /**
       * Post transition function
       * From stand -> left/right goes to running, up goes to jump
       */
      virtual void PostTransition();
   
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
      Player_Run(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();

      /**
       * Reset
       */
      virtual void TransitionReset();
};

class Player_Jump : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Jump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_Fall : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Fall(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_DoubleJump : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_DoubleJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform further action
       */
      virtual void PerformFurtherAction();

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_Push : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Push(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_JumpPush : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_JumpPush(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_Balance : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Balance(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Player_RunningJump : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_RunningJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pre transition
       */
      virtual void PreTransition();
};

class Player_Death : public PlayerState {
   public:
      /**
       * Constructor
       */
      Player_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform further action
       */
      virtual void PerformFurtherAction();

      /**
       * Animate function only goes to certain frame
       */
      virtual void Animate();

   private:
      // DoOnce object
      DoOnce do_once_;

      // Death timer
      SecondsTimer death_timer_;
};

/** ARM STATES **/

class ArmState : public DrawState {
   public:
      /**
       * Constructor
       */
      ArmState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player);

   protected:
      // Player ptr
      Player *player;
};

class Arm_Idle : public ArmState {
   public:
      /**
       * Constructor
       */
      Arm_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player);

      /**
       * Perform further action
       */
      virtual void PreTransition();

      /**
       * Initialize back to normal vector
       */
      virtual void TransitionInitialize();
};

class Arm_DoubleJump : public ArmState {
   public:
      /**
       * Constructor
       */
      Arm_DoubleJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player);

      /**
       * Perform further action function
       */
      virtual void PerformFurtherAction();

      /**
       * Pre transition
       */
      virtual void PreTransition();

      /**
       * Reset function
       */
      virtual void Reset();

   private:
      // Do once for setting reset frame
      DoOnce do_once_;
};

class Arm_Running : public ArmState {
   public:
      /**
       * Constructor
       */
      Arm_Running(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player);

      /**
       * Transition function
       */
      virtual void PreTransition();

      /**
       * Transition initialize for when coming from anything else?
       */
      virtual void TransitionInitialize();
};

class Arm_Shooting : public ArmState {
   public:
      /**
       * Constructor
       */
      Arm_Shooting(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player);

      /**
       * Transition function
       */
      virtual void PreTransition();
};

#endif