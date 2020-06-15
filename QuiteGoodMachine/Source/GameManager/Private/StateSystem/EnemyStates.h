#ifndef ENEMYSTATES_H_
#define ENEMYSTATES_H_

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h"

#include "QuiteGoodMachine/Source/GameManager/Private/DoOnce.h"

class Texture;
class DrawStateContext;
class Enemy;

class EnemyState : public DrawState {
   public:
      /**
       * Constructor
       */
      EnemyState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform Further action (should flip anims)
       */
      virtual void PerformAction();
   
   protected:
      // Player pointer
      Enemy *enemy;

   private:
      DoOnce do_once_death_;
};

class Enemy_Turn : public EnemyState {
   public:
      /**
       * Constructor
       */
      Enemy_Turn(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform further action (flips anims)
       */
      virtual void PerformFurtherAction();
};

/** Fecreez State **/

class Fecreez_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fecreez_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();

      /**
       * Reset function
       */
      virtual void TransitionReset();
};

class Fecreez_Attack : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fecreez_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Fecreez_Death : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fecreez_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);
};

/** Rosea **/

class Rosea_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Rosea_Hurt : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_Hurt(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Rosea_ArmIdle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_ArmIdle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Rosea_ArmAttack : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_ArmAttack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Rosea_ArmRetreat : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_ArmRetreat(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pre transition
       */
      virtual void PreTransition();
};

class Rosea_ArmHurt : public EnemyState {
   public:
      /**
       * Constructor
       */
      Rosea_ArmHurt(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

/** Mosquibler **/

class Mosquibler_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosquibler_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Mosquibler_Hit : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosquibler_Hit(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Mosquibler_Fall : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosquibler_Fall(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pretransition
       */
      virtual void PreTransition();
};

class Mosquibler_Death : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosquibler_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);
};

/** FRUIG **/

class Fruig_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fruig_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);
};

class Fruig_Death : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fruig_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Transition initialize
       */
      virtual void TransitionInitialize();
};

/** Fleet **/

class Fleet_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fleet_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);
};

class Fleet_Death : public EnemyState {
   public:
      /**
       * Constructor
       */
      Fleet_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);
};

/** Mosqueenbler **/

class Mosqueenbler_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosqueenbler_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class Mosqueenbler_Attack : public EnemyState {
   public:
      /**
       * Constructor
       */
      Mosqueenbler_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();

      /**
       * Transition reset
       */
      virtual void TransitionReset();
};

/** Mosquibler Egg **/

class MosquiblerEgg_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      MosquiblerEgg_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition();
};

class MosquiblerEgg_Attack : public EnemyState {
   public:
      /**
       * Constrcutor
       */
      MosquiblerEgg_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Perform further action
       */
      virtual void PerformFurtherAction();
};

/** Wormored **/

class Wormored_Idle : public EnemyState {
   public:
      /**
       * Constructor
       */
      Wormored_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition() {}
};

class Wormored_Attack : public EnemyState {
   public:
      /**
       * Constructor
       */
      Wormored_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition() {}
};

class Wormored_Excrete : public EnemyState {
   public:
      /**
       * Constructor
       */
      Wormored_Excrete(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition() {}
};

class Wormored_Sleep : public EnemyState {
   public:
      /**
       * Constructor
       */
      Wormored_Sleep(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition() {}
};

class Wormored_Awake : public EnemyState {
   public:
      /**
       * Constructor
       */
      Wormored_Awake(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * PreTransition
       */
      virtual void PreTransition() {}
};

#endif