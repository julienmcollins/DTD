#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/EnemyStates.h"
#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h"

#define KH Application::GetInstance().key_handler

Enemy_Turn::Enemy_Turn(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) 
{
   enemy = dynamic_cast<Enemy*>(context->GetBase());
}

void Enemy_Turn::PerformFurtherAction() {
   // Handle flipping if changing directions
   if (GetAnimation()->completed) {
      if (enemy->GetDirection() == TangibleElement::RIGHT) {
         if (KH.GetKeyPressed(KEY_LEFT) && !KH.GetKeyPressed(KEY_RIGHT)) {
            enemy->FlipAllAnimations();
         }
      } else if (enemy->GetDirection() == TangibleElement::LEFT) {
         if (KH.GetKeyPressed(KEY_RIGHT) && !KH.GetKeyPressed(KEY_LEFT)) {
            enemy->FlipAllAnimations();
         }
      }
   }
}

/** FECREEZ **/

Fecreez_Idle::Fecreez_Idle(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : EnemyState(context, texture, animation) {}

void Fecreez_Idle::PreTransition() {
   
}