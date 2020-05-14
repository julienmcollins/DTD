#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

DrawState::DrawState(StateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : StateInterface(context)
   , texture_(texture)
   , animation_(animation) 
{
   RenderingEngine::GetInstance().LoadResources(animation);
}

void DrawState::PerformAction() {
   // First, animate
   Animate();

   // Then, perform further action
   PerformFurtherAction();
}

void DrawState::PerformFurtherAction() {
   GetTexture()->Render(GetContext()->GetBase()->GetPosition(), GetContext()->GetBase()->GetAngle(), GetAnimation());
}

Texture *DrawState::GetTexture() {
   return texture_;
}

std::shared_ptr<Animation> DrawState::GetAnimation() {
   return animation_;
}

void DrawState::SetAnimation(std::shared_ptr<Animation> anim) {
   animation_ = anim;
}

void DrawState::Animate() {
   texture_->Animate(animation_.get());
}