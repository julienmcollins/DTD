#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

DrawState::DrawState(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation)
   : StateInterface(context)
   , texture_(texture)
   , animation_(animation)
   , draw_context_(context)
{
   RenderingEngine::GetInstance().LoadResources(animation);
}

void DrawState::PerformAction() {
   // First, animate
   Animate();

   // Draw
   Draw();

   // Turn if you need to
   Turn();

   // Then, perform further action
   PerformFurtherAction();
}

void DrawState::PerformFurtherAction() {}

void DrawState::SetState(std::string name) {
   GetContext()->SetState(GetContext()->GetState(name));
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

void DrawState::Draw() {
   GetContext()->GetBase()->draw_model_ = GetTexture()->Render(GetContext()->GetBase()->GetPosition(), GetContext()->GetBase()->GetAngle(), GetAnimation());
   // std::cout << "DrawState::Draw - draw_model_[3][0] = " << GetContext()->GetBase()->draw_model_[3][0] << std::endl;
}

void DrawState::Turn() {}

void DrawState::Reset() {
   animation_->curr_frame = animation_->reset_frame;
}

void DrawState::TransitionReset() {
   Reset();
}