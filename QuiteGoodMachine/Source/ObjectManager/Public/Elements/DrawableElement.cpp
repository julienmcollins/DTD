#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"

#include <string>
#include <memory>
#include <cstddef>

DrawableElement::DrawableElement(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : PositionalElement(name, initial_position, size) {}

Texture *DrawableElement::RegisterTexture(std::string file) {
   std::size_t found1 = file.find_first_of("/\\");
   std::size_t found2 = file.find(".png");
   std::string name = file.substr(found1 + 1, found2);
   main_texture_ = RenderingEngine::GetInstance().LoadTexture(name, file.c_str());
   return main_texture_;
}

void DrawableElement::Update(bool freeze) {
   // First, animate
   Animate();

   // Then, draw
   Draw();
}

// TODO: Add a MediaFile with specific format
void DrawableElement::LoadMedia() {}

void DrawableElement::Draw() {
   // Render to screen based on main texture
   main_texture_->Render(GetPosition(), 0.0f, GetAnimationFromState());
}

void DrawableElement::Animate() {}

std::shared_ptr<Animation> DrawableElement::GetAnimationByName(std::string name) {}

std::shared_ptr<Animation> DrawableElement::GetAnimationFromState() {
   DrawState *temp = static_cast<DrawState *>(GetStateContext()->GetCurrentState().get());
   return temp->GetAnimation();
}

void DrawableElement::FlipAnimation(std::string name) {}

void DrawableElement::FlipAllAnimations() {}

bool DrawableElement::AllAnimationsFlipped() {}

std::string DrawableElement::GetType() {
   return "DrawableElement";
}

DrawableElement::~DrawableElement() {}