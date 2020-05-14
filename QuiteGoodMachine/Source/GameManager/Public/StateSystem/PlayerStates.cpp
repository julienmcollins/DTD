#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/PlayerStates.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include <cmath>

#define KH Application::GetInstance().key_handler

/** PLAYERSTATE **/
PlayerState::PlayerState(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   DrawState(context, texture, animation) 
{
   player = dynamic_cast<Player*>(GetContext()->GetBase());
}

void PlayerState::PerformFurtherAction() {
   // Handle flipping if changing directions
   if (player->GetDirection() == TangibleElement::RIGHT) {
      if (KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
         player->FlipAllAnimations();
      }
   } else if (player->GetDirection() == TangibleElement::LEFT) {
      if (KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
         player->FlipAllAnimations();
      }
   }
}

/** PLAYER_STAND **/
Player_Stand::Player_Stand(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation1, std::shared_ptr<Animation> animation2, std::shared_ptr<Animation> animation3)
   : PlayerState(context, texture, animation1)
   , anim_1_(animation1)
   , anim_2_(animation2)
   , anim_3_(animation3) {}

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
}

void Player_Stand::PreTransition() {
   // Check if balance limit exceeded
   if (balance_timer_.GetTime() > 1.0) {
      balance_timer_.Stop();
      balance_timer_.Reset();
      GetContext()->SetState(GetContext()->GetState("balance"));
   }
}

void Player_Stand::PostTransition() {
   // If facing opposite direction of key press, flip anim
   if (player->GetDirection() == TangibleElement::RIGHT && KH.GetKeyPressed(KEY_LEFT)) {
      player->FlipAllAnimations();
      GetContext()->SetState(GetContext()->GetState("running"));
   } else if (player->GetDirection() == TangibleElement::LEFT && KH.GetKeyPressed(KEY_RIGHT)) {
      player->FlipAllAnimations();
      GetContext()->SetState(GetContext()->GetState("running"));
   } else if (KH.GetKeyPressed(KEY_RIGHT) || KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
   }

   // Deal with jump
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("jump"));
   }

   // Deal with pushing
   if (player->GetDirection() == TangibleElement::RIGHT && player->contacts_[player->RIGHT_ARM] && KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("push"));
   } else if (player->GetDirection() == TangibleElement::LEFT && player->contacts_[player->LEFT_ARM] && KH.GetKeyPressed(KEY_LEFT)) {
      GetContext()->SetState(GetContext()->GetState("push"));
   }
}

/** PLAYER_RUN **/
Player_Run::Player_Run(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Stand::PreTransition() {
   // Handle no key pressed --> transition to idle
     if (!KH.GetKeyPressed(KEY_UP) && !KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("stand"));
   }

   // Handle jump
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("running_jump"));
   }

   // Handle wall push
   if (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM]
    || KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) {
      GetContext()->SetState(GetContext()->GetState("push"));
   }

   // Handle running off ledge
   if (!player->contacts_[player->LEFT_LEG] && !player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("fall"));
   }
}

/** PLAYER_JUMP **/
Player_Jump::Player_Jump(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Jump::PreTransition() {
   // Handle double jump
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("double_jump"));
   }

   // Handle wall push
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM])
    || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
   }
}

/** PLAYER_DOUBLEJUMP **/
Player_DoubleJump::Player_DoubleJump(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_DoubleJump::PreTransition() {
   // Handle wall push
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
   }
}

/** PLAYER_PUSH **/
Player_Push::Player_Push(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Push::PreTransition() {
   // Handle stop pushing
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         GetContext()->SetState(GetContext()->GetState("running"));
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         GetContext()->SetState(GetContext()->GetState("running"));
      }
   }

   // Handle jump push
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("jump_push"));
   }
}

/** PLAYER_JUMPPUSH **/
Player_JumpPush::Player_JumpPush(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_JumpPush::PreTransition() {
   // Handle detach from wall
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         GetContext()->SetState(GetContext()->GetState("jump"));
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         GetContext()->SetState(GetContext()->GetState("jump"));
      }
   }

   // Handle land
   if ((player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG])) {
      if (!(KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT))) {
         GetContext()->SetState(GetContext()->GetState("stand"));
      } else { // Might need to refine this else
         GetContext()->SetState(GetContext()->GetState("push"));
      }
   }
}

/** PLAYER_BALANCE **/
Player_Balance::Player_Balance(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Balance::PreTransition() {
   // Handle jump
   if (KH.GetKeyPressed(KEY_UP)) {
      GetContext()->SetState(GetContext()->GetState("jump"));
   }

   // Handle run
   if (KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT)) {
      GetContext()->SetState(GetContext()->GetState("running"));
   }
}

/** PLAYER_FALL **/
Player_Fall::Player_Fall(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

void Player_Fall::PreTransition() {
   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      GetContext()->SetState(GetContext()->GetState("stand"));
   }
}

/** PLAYER_DEATH **/
Player_Death::Player_Death(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(context, texture, animation) {}

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