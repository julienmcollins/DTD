#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/EnemyStates.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h"

#define KH Application::GetInstance().key_handler

/** ENEMY STATE **/
EnemyState::EnemyState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : DrawState(context, texture, animation) 
{
   enemy = dynamic_cast<Enemy*>(context->GetBase());
}

void EnemyState::PerformAction() {
   // First, animate
   Animate();

   // Draw
   Draw();

   // Turn if you need to
   Turn();

   // If marked for death, immediately go to death anim
   if (enemy->MarkedForDeath()) {
      do_once_death_([&]() {
         GetContext()->SetState(GetContext()->GetState("death"));
      });
   }

   // Then, perform further action
   PerformFurtherAction();
}


/** ENEMY TURN **/
Enemy_Turn::Enemy_Turn(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) {}

void Enemy_Turn::PerformFurtherAction() {
   // Handle flipping if changing directions
   if (GetAnimation()->completed) {
      if (Application::GetInstance().get_player()->GetPosition().x <= enemy->GetPosition().x && 
          enemy->GetDirection() == TangibleElement::RIGHT)
      {
         static_cast<DrawStateContext*>(GetContext())->FlipAllAnimations();
         enemy->SetDirection(TangibleElement::LEFT);
         GetContext()->SetState(GetContext()->GetState("idle"));
      } else if (Application::GetInstance().get_player()->GetPosition().x > enemy->GetPosition().x &&
                 enemy->GetDirection() == TangibleElement::LEFT) 
      {
         static_cast<DrawStateContext*>(GetContext())->FlipAllAnimations();
         enemy->SetDirection(TangibleElement::RIGHT);
         GetContext()->SetState(GetContext()->GetState("idle"));
      }
   }
}

/** FECREEZ **/

Fecreez_Idle::Fecreez_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Fecreez*>(context->GetBase());
}

void Fecreez_Idle::PreAction() {
   if (Application::GetInstance().get_player()->GetPosition().y >= enemy->GetPosition().y - enemy->GetSize().y &&
       Application::GetInstance().get_player()->GetPosition().y <= enemy->GetPosition().y + enemy->GetSize().y)
   {
      if (enemy->shoot_timer_ >= 100) {
         GetContext()->SetState(GetContext()->GetState("attack"));
         return;
      }
   }
}

void Fecreez_Idle::PreTransition() {
   DrawState::PreTransition();
   enemy->shoot_timer_ = 0;
}

Fecreez_Attack::Fecreez_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Fecreez*>(context->GetBase());
}

void Fecreez_Attack::PreAction() {
   if (Application::GetInstance().get_player()->GetPosition().y >= enemy->GetPosition().y - enemy->GetSize().y &&
       Application::GetInstance().get_player()->GetPosition().y <= enemy->GetPosition().y + enemy->GetSize().y)
   {
      if (enemy->shoot_timer_ < 100 && GetAnimation()->completed) {
         GetContext()->SetState(GetContext()->GetState("idle"));
         return;
      }
   }
}

Fecreez_Death::Fecreez_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Fecreez*>(context->GetBase());
}

/** ROSEA **/

Rosea_Idle::Rosea_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
}

void Rosea_Idle::PreAction() {
   if (enemy->was_hurt) {
      GetContext()->SetState(GetContext()->GetState("hurt"));
      return;
   }
}

Rosea_Hurt::Rosea_Hurt(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
}

void Rosea_Hurt::PreAction() {
   if (GetAnimation()->completed) {
      GetContext()->SetState(GetContext()->GetState("idle"));
      return;
   }
}

Rosea_ArmIdle::Rosea_ArmIdle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
}

void Rosea_ArmIdle::PreAction() {
   if (enemy->was_hurt) {
      GetContext()->SetState(GetContext()->GetState("arm_hurt"));
      return;
   }

   if (enemy->within_bounds()) {
      GetContext()->SetState(GetContext()->GetState("arm_attack"));
      return;
   }
}

Rosea_ArmAttack::Rosea_ArmAttack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
}

void Rosea_ArmAttack::PreAction() {
   if (!enemy->within_bounds()) {
      SetState("retreat");
      return;
   }
}

Rosea_ArmRetreat::Rosea_ArmRetreat(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
   GetAnimation()->reset_frame = 7;
}

void Rosea_ArmRetreat::PreAction() {
   if (enemy->within_bounds()) {
      SetState("attack");
      return;
   }

   if (GetAnimation()->completed) {
      SetState("idle");
      return;
   }
}

Rosea_ArmHurt::Rosea_ArmHurt(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Rosea*>(context->GetBase());
}

void Rosea_ArmHurt::PreAction() {
   if (enemy->within_bounds() && GetAnimation()->completed) {
      SetState("attack");
      return;
   }

   if (GetAnimation()->completed) {
      GetContext()->SetState(GetContext()->GetState("idle"));
      return;
   }
}

/** MOSQUIBLER **/

Mosquibler_Idle::Mosquibler_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosquibler*>(context->GetBase());
}

void Mosquibler_Idle::PreAction() {
   if (Application::GetInstance().get_player()->GetPosition().x <= enemy->GetPosition().x && enemy->GetDirection() == TangibleElement::RIGHT) {
      SetState("turn");
      return;
   } else if (Application::GetInstance().get_player()->GetPosition().x > enemy->GetPosition().x && enemy->GetDirection() == TangibleElement::LEFT) {
      SetState("turn");
      return;
   }

   if (enemy->was_hurt) {
      SetState("hit");
      return;
   }
}

Mosquibler_Hit::Mosquibler_Hit(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosquibler*>(context->GetBase());
}

void Mosquibler_Hit::PreAction() {
   if (GetAnimation()->completed) {
      SetState("fall");
      return;
   }
}

Mosquibler_Fall::Mosquibler_Fall(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosquibler*>(context->GetBase());
}

void Mosquibler_Fall::PreAction() {
   if (enemy->hit_ground) {
      enemy->SetMarkedForDeath();
      return;
   }
}

Mosquibler_Death::Mosquibler_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosquibler*>(context->GetBase());
}

/** FRUIG **/

Fruig_Idle::Fruig_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Fruig*>(context->GetBase());
}

Fruig_Death::Fruig_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Fruig*>(context->GetBase());
}

void Fruig_Death::PostTransition() {
   GetAnimation()->reset_frame = 14;
}

/** FLEET **/

Fleet_Idle::Fleet_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Fleet*>(context->GetBase());
}

Fleet_Death::Fleet_Death(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Fleet*>(context->GetBase());
}

/** MOSQUEENBLER **/

Mosqueenbler_Idle::Mosqueenbler_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosqueenbler*>(context->GetBase());
}

void Mosqueenbler_Idle::PreAction() {
   if (enemy->shoot_timer_ > 200) {
      SetState("attack");
      return;
   }
}

Mosqueenbler_Attack::Mosqueenbler_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Mosqueenbler*>(context->GetBase());
}

void Mosqueenbler_Attack::PreAction() {
   if (GetAnimation()->completed) {
      SetState("attack");
   }
}

void Mosqueenbler_Attack::PreTransition() {
   DrawState::PreTransition();
   enemy->shoot_timer_ = 0;
}

/** MOSQUIBLER EGG **/

MosquiblerEgg_Idle::MosquiblerEgg_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<MosquiblerEgg*>(context->GetBase());
}

void MosquiblerEgg_Idle::PreAction() {
   if (enemy->hit_ground) {
      SetState("attack");
      return;
   }
}

MosquiblerEgg_Attack::MosquiblerEgg_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<MosquiblerEgg*>(context->GetBase());
}

void MosquiblerEgg_Attack::PerformFurtherAction() {
   if (GetAnimation()->completed) {
      enemy->SetIsAlive(false);
   }
}

/** WORMORED **/

Wormored_Idle::Wormored_Idle(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Wormored*>(context->GetBase());
}

Wormored_Attack::Wormored_Attack(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Wormored*>(context->GetBase());
}

Wormored_Excrete::Wormored_Excrete(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Wormored*>(context->GetBase());
}

Wormored_Sleep::Wormored_Sleep(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Wormored*>(context->GetBase());
}

Wormored_Awake::Wormored_Awake(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation) 
   : EnemyState(context, texture, animation)
{
   enemy = dynamic_cast<Wormored*>(context->GetBase());
}