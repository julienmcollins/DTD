#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/PlayerStates.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Entity.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include <cmath>

#define KH Application::GetInstance().key_handler

/** PLAYERSTATE **/
PlayerState::PlayerState(Texture *texture, std::shared_ptr<Animation> animation) :
   DrawState(texture, animation) {}

void PlayerState::PerformFurtherAction(StateContext *context) {
   // Handle flipping if changing directions
   Player* player = static_cast<Player*>(context->GetBase());
   if (player->entity_direction == player->RIGHT) {
      if (KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
         player->FlipAllAnimations();
      }
   } else if (player->entity_direction == player->LEFT) {
      if (KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
         player->FlipAllAnimations();
      }
   }
}

/** PLAYER_STAND **/
Player_Stand::Player_Stand(Texture *texture, std::shared_ptr<Animation> animation1, std::shared_ptr<Animation> animation2, std::shared_ptr<Animation> animation3)
   : PlayerState(texture, animation1)
   , anim_1_(animation1)
   , anim_2_(animation2)
   , anim_3_(animation3) {}

void Player_Stand::PerformFurtherAction(StateContext *context) {
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
   Player* player = static_cast<Player*>(context->GetBase());
   if ((player->contacts_[player->LEFT_LEG] && !player->contacts_[player->RIGHT_LEG])
      || (player->contacts_[player->RIGHT_LEG] && !player->contacts_[player->LEFT_LEG])) {
      balance_timer_.Start();
   }
}

void Player_Stand::PreTransition(StateContext *context) {
   // Check if balance limit exceeded
   if (balance_timer_.GetTime() > 1.0) {
      balance_timer_.Stop();
      balance_timer_.Reset();
      context->SetState(context->GetState("balance"));
   }
}

void Player_Stand::PostTransition(StateContext *context) {
   // Get player...
   Player* player = static_cast<Player*>(context->GetBase());

   // If facing opposite direction of key press, flip anim
   if (player->entity_direction == Player::RIGHT && player->KEY_LEFT) {
      player->FlipAllAnimations();
      context->SetState(context->GetState("running"));
   } else if (player->entity_direction == Player::LEFT && player->KEY_RIGHT) {
      player->FlipAllAnimations();
      context->SetState(context->GetState("running"));
   } else if (player->KEY_RIGHT || player->KEY_LEFT) {
      context->SetState(context->GetState("running"));
   }

   // Deal with jump
   if (player->KEY_UP) {
      context->SetState(context->GetState("jump"));
   }

   // Deal with pushing
   if (player->entity_direction == Player::RIGHT && player->contacts_[player->RIGHT_ARM] && player->KEY_RIGHT) {
      context->SetState(context->GetState("push"));
   } else if (player->entity_direction == Player::LEFT && player->contacts_[player->LEFT_ARM] && player->KEY_LEFT) {
      context->SetState(context->GetState("push"));
   }
}

/** PLAYER_RUN **/
Player_Run::Player_Run(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Stand::PreTransition(StateContext *context) {
   // Handle no key pressed --> transition to idle
   Player* player = static_cast<Player*>(context->GetBase());
   if (!KH.GetKeyPressed(KEY_UP) && !KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
      context->SetState(context->GetState("stand"));
   }

   // Handle jump
   if (KH.GetKeyPressed(KEY_UP)) {
      context->SetState(context->GetState("running_jump"));
   }

   // Handle wall push
   if (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM]
    || KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) {
      context->SetState(context->GetState("push"));
   }

   // Handle running off ledge
   if (!player->contacts_[player->LEFT_LEG] && !player->contacts_[player->RIGHT_LEG]) {
      context->SetState(context->GetState("fall"));
   }
}

/** PLAYER_JUMP **/
Player_Jump::Player_Jump(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Jump::PreTransition(StateContext *context) {
   // Handle double jump
   Player *player = static_cast<Player*>(context->GetBase());
   if (KH.GetKeyPressed(KEY_UP)) {
      context->SetState(context->GetState("double_jump"));
   }

   // Handle wall push
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM])
    || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      context->SetState(context->GetState("jump_push"));
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      context->SetState(context->GetState("stand"));
   }
}

/** PLAYER_DOUBLEJUMP **/
Player_DoubleJump::Player_DoubleJump(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_DoubleJump::PreTransition(StateContext *context) {
   // Handle wall push
   Player *player = static_cast<Player*>(context->GetBase());
   if ((KH.GetKeyPressed(KEY_LEFT) && player->contacts_[player->LEFT_ARM]) || (KH.GetKeyPressed(KEY_RIGHT) && player->contacts_[player->RIGHT_ARM])) {
      context->SetState(context->GetState("jump_push"));
   }

   // Handle land
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      context->SetState(context->GetState("stand"));
   }
}

/** PLAYER_PUSH **/
Player_Push::Player_Push(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Push::PreTransition(StateContext *context) {
   // Handle stop pushing
   Player *player = static_cast<Player*>(context->GetBase());
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         context->SetState(context->GetState("running"));
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         context->SetState(context->GetState("running"));
      }
   }

   // Handle jump push
   if (KH.GetKeyPressed(KEY_UP)) {
      context->SetState(context->GetState("jump_push"));
   }
}

/** PLAYER_JUMPPUSH **/
Player_JumpPush::Player_JumpPush(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_JumpPush::PreTransition(StateContext *context) {
   // Handle detach from wall
   Player *player = static_cast<Player*>(context->GetBase());
   if (player->contacts_[player->LEFT_ARM]) {
      if (KH.GetKeyPressed(KEY_RIGHT)) {
         context->SetState(context->GetState("jump"));
      }
   } else if (player->contacts_[player->RIGHT_ARM]) {
      if (KH.GetKeyPressed(KEY_LEFT)) {
         context->SetState(context->GetState("jump"));
      }
   }

   // Handle land
   if ((player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG])) {
      if (!(KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT))) {
         context->SetState(context->GetState("stand"));
      } else { // Might need to refine this else
         context->SetState(context->GetState("push"));
      }
   }
}

/** PLAYER_BALANCE **/
Player_Balance::Player_Balance(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Balance::PreTransition(StateContext *context) {
   // Handle jump
   Player *player = static_cast<Player*>(context->GetBase());
   if (KH.GetKeyPressed(KEY_UP)) {
      context->SetState(context->GetState("jump"));
   }

   // Handle run
   if (KH.GetKeyPressed(KEY_LEFT) || KH.GetKeyPressed(KEY_RIGHT)) {
      context->SetState(context->GetState("running"));
   }
}

/** PLAYER_FALL **/
Player_Fall::Player_Fall(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Fall::PreTransition(StateContext *context) {
   // Handle land
   Player *player = static_cast<Player*>(context->GetBase());
   if (player->contacts_[player->LEFT_LEG] || player->contacts_[player->RIGHT_LEG]) {
      context->SetState(context->GetState("stand"));
   }
}

/** PLAYER_DEATH **/
Player_Death::Player_Death(Texture *texture, std::shared_ptr<Animation> animation) :
   PlayerState(texture, animation) {}

void Player_Death::Animate() {
   GetTexture()->Animate(GetAnimation().get(), 19, 19);
}