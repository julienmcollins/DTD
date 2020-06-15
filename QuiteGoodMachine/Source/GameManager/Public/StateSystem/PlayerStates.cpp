#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/PlayerStates.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

#include <cmath>

#define KH Application::GetInstance().key_handler

/** PLAYERSTATE **/
PlayerState::PlayerState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   DrawState(context, texture, animation) 
{
   player = dynamic_cast<Player*>(GetContext()->GetBase());
}

void PlayerState::Turn() {
   // Handle flipping if changing directions
   if (player->GetDirection() == TangibleElement::RIGHT) {
      if (KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
         GetContext()->FlipAllAnimations();
         player->arm_.GetStateContext()->FlipAllAnimations();
         player->arm_.SetPosition(glm::vec3(-player->arm_.GetPosition().x, player->arm_.GetPosition().y, 0.f));
         player->SetDirection(TangibleElement::LEFT);
      }
   } else if (player->GetDirection() == TangibleElement::LEFT) {
      if (KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
         GetContext()->FlipAllAnimations();
         player->arm_.GetStateContext()->FlipAllAnimations();
         player->arm_.SetPosition(glm::vec3(-player->arm_.GetPosition().x, player->arm_.GetPosition().y, 0.f));
         player->SetDirection(TangibleElement::RIGHT);
      }
   }
}

/** PLAYER_STAND **/
Player_Stand::Player_Stand(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation1, std::shared_ptr<Animation> animation2, std::shared_ptr<Animation> animation3)
   : PlayerState(context, texture, animation1)
   , anim_1_(animation1)
   , anim_2_(animation2)
   , anim_3_(animation3) 
{
   RenderingEngine::GetInstance().LoadResources(animation2);
   RenderingEngine::GetInstance().LoadResources(animation3);
}

void Player_Stand::PerformFurtherAction() {
   // Change anim by chance
   if (GetAnimation()->curr_frame >= GetAnimation()->max_frame) {
      int rand_idle = rand() % 100 + 1;

      // Set different texture
      if (rand_idle <= 98) {
         SetAnimation(anim_1_);
      }
      if (rand_idle == 99) {
         SetAnimation(anim_2_);
      }
      if (rand_idle == 100) {
         SetAnimation(anim_3_);
      }
   }

   // Start timer if on ledge
   if ((player->contacts_[player->LEFT_LEG] && !player->contacts_[player->RIGHT_LEG])
      || (player->contacts_[player->RIGHT_LEG] && !player->contacts_[player->LEFT_LEG])) {
      balance_timer_.Start();
   }

   // Perform further action if needed
}

void Player_Stand::PreTransition() {
   // If facing opposite direction of key press, flip anim
   if (player->GetDirection() == TangibleElement::RIGHT && KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
      return;
   } else if (player->GetDirection() == TangibleElement::LEFT && KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
      return;
   } else if (KH.GetKeyPressed(KEY_RIGHT) || KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
      return;
   }

   // Deal with jump
   if (KH.GetKeyPressed(KEY_UP) && KH.KeyIsLocked(KEY_UP)) {
      PlayerState *j = static_cast<PlayerState*>(GetContext()->GetState("jump").get());
      j->GetAnimation()->reset_frame = j->GetAnimation()->max_frame;
      j->GetAnimation()->stop_frame = j->GetAnimation()->max_frame;
      GetContext()->SetState(GetContext()->GetState("jump"));
      return;
   }

   // Deal with pushing
   if (player->GetDirection() == TangibleElement::RIGHT && player->contacts_[player->RIGHT_ARM] && KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("push"));
      return;
   } else if (player->GetDirection() == TangibleElement::LEFT && player->contacts_[player->LEFT_ARM] && KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("push"));
      return;
   }
}

void Player_Stand::PostTransition() {
   // Check if balance limit exceeded
   if (balance_timer_.GetTime() > 1.0) {
      balance_timer_.Stop();
      balance_timer_.Reset();
      GetContext()->SetState(GetContext()->GetState("balance"));
      return;
   }
}

/** PLAYER_RUN **/
Player_Run::Player_Run(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Run::PreTransition() {
   // Handle no key pressed --> transition to idle
     if (!KH.GetKeyPressed(KEY_UP) && !KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("stand"));
      return;
   }

   // Handle jump
   if (KH.GetKeyPressed(KEY_UP) && KH.KeyIsLocked(KEY_UP)) {
      PlayerState *rj = static_cast<PlayerState*>(GetContext()->GetState("running_jump").get());
      rj->GetAnimation()->reset_frame = 14;
      rj->GetAnimation()->stop_frame = 14;
      GetContext()->SetState(GetContext()->GetState("running_jump"));
      return;
   }

   // Handle wall push
   if (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM]
    || KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) {
      GetContext()->SetState(GetContext()->GetState("push"));
      return;
   }

   // Handle running off ledge
   if (!player->contacts_[player->LEFT_LEG] && !player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("fall"));
      return;
   }
}

void Player_Run::TransitionReset() {
   PlayerState *r = static_cast<PlayerState*>(player->arm_.GetStateContext()->GetState("running").get());
   r->GetAnimation()->curr_frame = GetAnimation()->curr_frame;
}

/** PLAYER_JUMP **/
Player_Jump::Player_Jump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Jump::PreTransition() {
   // Handle double jump
   if (KH.GetKeyPressed(KEY_UP) && KH.KeyIsLocked(KEY_UP) && player->has_jumped_ > 1) {
      PlayerState *dj = static_cast<PlayerState*>(GetContext()->GetState("double_jump").get());
      dj->GetAnimation()->reset_frame = dj->GetAnimation()->max_frame;
      dj->GetAnimation()->stop_frame = dj->GetAnimation()->max_frame;
      GetContext()->SetState(GetContext()->GetState("double_jump"));
      return;
   }

   // Handle wall push
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM])
    || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
      return;
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
      return;
   }
}

/** PLAYER_DOUBLEJUMP **/
Player_DoubleJump::Player_DoubleJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_DoubleJump::PerformFurtherAction() {
   if ((KH.GetKeyPressed(KEY_RIGHT) && player->GetDirection() == player->LEFT) ||
       (KH.GetKeyPressed(KEY_LEFT) && player->GetDirection() == player->RIGHT)) 
   {
      b2Vec2 vel = {0.0f, player->GetBody()->GetLinearVelocity().y};
      player->GetBody()->SetLinearVelocity(vel);
   }
}

void Player_DoubleJump::PreTransition() {
   // Handle wall push
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
      return;
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
      return;
   }
}

/** PLAYER_PUSH **/
Player_Push::Player_Push(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Push::PreTransition() {
   // Handle stop pushing
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         GetContext()->SetState(GetContext()->GetState("running"));
         return;
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         GetContext()->SetState(GetContext()->GetState("running"));
         return;
      }
   }

   // Handle jump push
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
      return;
   }
}

/** PLAYER_JUMPPUSH **/
Player_JumpPush::Player_JumpPush(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_JumpPush::PreTransition() {
   // Handle detach from wall
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         PlayerState *j = static_cast<PlayerState*>(GetContext()->GetState("jump").get());
         j->GetAnimation()->reset_frame = j->GetAnimation()->max_frame;
         j->GetAnimation()->stop_frame = j->GetAnimation()->max_frame;
         GetContext()->SetState(GetContext()->GetState("jump"));
         return;
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         PlayerState *j = static_cast<PlayerState*>(GetContext()->GetState("jump").get());
         j->GetAnimation()->reset_frame = j->GetAnimation()->max_frame;
         j->GetAnimation()->stop_frame = j->GetAnimation()->max_frame;
         GetContext()->SetState(GetContext()->GetState("jump"));
         return;
      }
   }

   // Handle land
   if ((player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG])) {
      if (!(KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT))) {
         GetContext()->SetState(GetContext()->GetState("stand"));
         return;
      } else { // Might need to refine this else
         GetContext()->SetState(GetContext()->GetState("push"));
         return;
      }
   }
}

/** PLAYER_BALANCE **/
Player_Balance::Player_Balance(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Balance::PreTransition() {
   // Handle jump
   if (KH.GetKeyPressed(KEY_UP)) {
      PlayerState *j = static_cast<PlayerState*>(GetContext()->GetState("jump").get());
      j->GetAnimation()->reset_frame = j->GetAnimation()->max_frame;
      j->GetAnimation()->stop_frame = j->GetAnimation()->max_frame;
      GetContext()->SetState(GetContext()->GetState("jump"));
      return;
   }

   // Handle run
   if (KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
      return;
   }
}

/** PLAYER_FALL **/
Player_Fall::Player_Fall(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Fall::PreTransition() {
   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
      return;
   }
}

/** PLAYER_RUNNING_JUMP **/
Player_RunningJump::Player_RunningJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : PlayerState(context, texture, animation) {}

void Player_RunningJump::PreTransition() {
   // std::cout << "Player_RunningJump::PreTransition - has jumped = " << player->has_jumped_ << std::endl;
   if (KH.GetKeyPressed(KEY_UP) && KH.KeyIsLocked(KEY_UP) && player->has_jumped_ > 1) {
      PlayerState *dj = static_cast<PlayerState*>(GetContext()->GetState("double_jump").get());
      dj->GetAnimation()->reset_frame = dj->GetAnimation()->max_frame;
      dj->GetAnimation()->stop_frame = dj->GetAnimation()->max_frame;
      GetContext()->SetState(GetContext()->GetState("double_jump"));
      return;
   }

   if (player->contacts_[player->LEFT_ARM] || player->contacts_[player->RIGHT_ARM]) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
      return;
   }

   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      if (KH.GetKeyPressed(KEY_RIGHT) || KH.GetKeyPressed(KEY_LEFT)) {
         GetContext()->SetState(GetContext()->GetState("running"));
         return;
      } else {
         GetContext()->SetState(GetContext()->GetState("stand"));
         return;
      }
   }
}

/** PLAYER_DEATH **/
Player_Death::Player_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : PlayerState(context, texture, animation) {}

void Player_Death::PerformFurtherAction() {
   do_once_([&]() {
      player->GetBody()->SetLinearVelocity({0.0f, player->GetBody()->GetLinearVelocity().y});
      death_timer_.Start();
      player->SetCollision(player->CAT_PLATFORM, player->GetMainFixture());
   });
   if (GetAnimation()->curr_frame >= 19) {
      if (death_timer_.GetTime() > 3.0) {
         player->SetIsAlive(false);
         death_timer_.Stop();
      }
   }
}

void Player_Death::Animate() {
   GetTexture()->Animate(GetAnimation().get(), 19, 19);
}

/**************/

ArmState::ArmState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player)
   : DrawState(context, texture, animation)
   , player(player) {}

/** ARM IDLE **/
Arm_Idle::Arm_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player)
   : ArmState(context, texture, animation, player) {}

void Arm_Idle::PreTransition() {
   if (KH.GetKeyPressed(KEY_RIGHT) || KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
      std::cout << "Arm_Idle::PreTransition - transitioning to running\n";
      return;
   }

   if (KH.GetKeyPressed(KEY_SPACE)) {
      GetContext()->SetState(GetContext()->GetState("shooting"));
      return;
   }

   if (player->GetStateContext()->GetCurrentState() == player->GetStateContext()->GetState("jump") && KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("double_jump"));
      return;
   }
}

void Arm_Idle::TransitionInitialize() {
   float x = player->GetDirection() == TangibleElement::RIGHT ? 9.5f : -9.5f;
   GetContext()->GetBase()->SetPosition(glm::vec3(x, 6.f, 0.f));
}

/** ARM_RUNNING **/
Arm_Running::Arm_Running(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player)
   : ArmState(context, texture, animation, player) {}

void Arm_Running::PreTransition() {
   if (!KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("idle"));
      return;
   }

   if (KH.GetKeyPressed(KEY_SPACE)) {
      GetContext()->SetState(GetContext()->GetState("shooting"));
      return;
   }
}

void Arm_Running::TransitionInitialize() {
   float offset = player->GetDirection() == TangibleElement::RIGHT ? 10.f : -10.f;
   GetContext()->GetBase()->SetPosition(glm::vec3(GetContext()->GetBase()->GetPosition().x + offset, GetContext()->GetBase()->GetPosition().y, 0.f));
}

/** ARM_DOUBLEJUMP **/
Arm_DoubleJump::Arm_DoubleJump(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player)
   : ArmState(context, texture, animation, player) {}

void Arm_DoubleJump::PerformFurtherAction() {
   do_once_([&]() {
      GetAnimation()->reset_frame = GetAnimation()->max_frame;
   });
}

void Arm_DoubleJump::PreTransition() {
   if (KH.GetKeyPressed(KEY_SPACE)) {
      GetContext()->SetState(GetContext()->GetState("shooting"));
      return;
   }

   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("idle"));
      return;
   }
}

void Arm_DoubleJump::Reset() {
   GetAnimation()->reset_frame = 0;
   DrawState::Reset();
   do_once_.Reset();
}

/** ARM_SHOOTING **/
Arm_Shooting::Arm_Shooting(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation, Player *player)
   : ArmState(context, texture, animation, player) {}

void Arm_Shooting::PreTransition() {
   if (GetAnimation()->completed && !KH.GetKeyPressed(KEY_SPACE)) {
      GetContext()->SetState(GetContext()->GetState("idle"));
      return;
   }
}