#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

DrawState::DrawState(Texture *texture, std::shared_ptr<Animation> animation)
   : texture_(texture)
   , animation_(animation) 
{
   RenderingEngine::GetInstance().LoadResources(animation);
}

void DrawState::PerformAction(StateContext *context) {
   // First, animate
   Animate();

   // Then, perform further action
   PerformFurtherAction(context);
}

void DrawState::PerformFurtherAction(StateContext *context) {
   GetTexture()->Render(context->GetBase()->GetPosition(), context->GetBase()->GetAngle(), GetAnimation());
}

Texture *DrawState::GetTexture() {
   return texture_;
}

std::shared_ptr<Animation> DrawState::GetAnimation() {
   return animation_;
}

void DrawState::Animate() {
   texture_->Animate(animation_.get());
}