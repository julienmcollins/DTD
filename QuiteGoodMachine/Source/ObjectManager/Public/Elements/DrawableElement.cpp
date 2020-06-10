#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

#include <string>
#include <memory>
#include <cstddef>
#include <iostream>

using namespace std;

DrawableElement::DrawableElement(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : PositionalElement(name, initial_position, size)
   , main_texture_(nullptr)
{
   state_context_ = std::make_shared<DrawStateContext>(this);
   draw_model_ = glm::mat4(1.0f);

   // Translate over by amount
   draw_model_ = glm::translate(draw_model_, initial_position);
}

Texture *DrawableElement::RegisterTexture(std::string file) {
   std::size_t found1 = file.find_last_of("/\\");
   std::size_t found2 = file.find_last_of(".");
   std::string name = file.substr(found1 + 1, found2 - (found1 + 1));
   if (!main_texture_) {
      main_texture_ = RenderingEngine::GetInstance().LoadTexture(name, file.c_str());
      return main_texture_;
   } else {
      additional_textures_.emplace(pair<string, Texture*>(name, RenderingEngine::GetInstance().LoadTexture(name, file.c_str())));
      return additional_textures_[name];
   }
}

void DrawableElement::Update(bool freeze) {
   // Rely on state
   GetStateContext()->DoAction();
}

// TODO: Add a MediaFile with specific format
void DrawableElement::LoadMedia() {}

void DrawableElement::Draw() {
   // Render to screen based on main texture
   main_texture_->Render(GetPosition(), GetAngle(), GetAnimationFromState());
}

void DrawableElement::Animate() {}

std::shared_ptr<Animation> DrawableElement::GetAnimationByName(std::string name) {}

std::shared_ptr<Animation> DrawableElement::GetAnimationFromState() {
   DrawState *temp = static_cast<DrawState *>(GetStateContext()->GetCurrentState().get());
   return temp->GetAnimation();
}

std::shared_ptr<DrawStateContext> DrawableElement::GetStateContext() {
   return std::static_pointer_cast<DrawStateContext>(state_context_);
}

void DrawableElement::FlipAnimation(std::string name) {}

void DrawableElement::FlipAllAnimations() {}

bool DrawableElement::AllAnimationsFlipped() {}

std::string DrawableElement::GetType() {
   return "DrawableElement";
}

DrawableElement::~DrawableElement() {}