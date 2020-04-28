#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"

#include <string>
#include <memory>

DrawableElement::DrawableElement(std::string name, glm::vec2 initial_position, glm::vec2 size)
   : PositionalElement(name, initial_position, size) {}

void DrawableElement::Update(bool freeze) {}

void DrawableElement::LoadMedia() {}

void DrawableElement::Draw() {}

void DrawableElement::Animate() {}

std::shared_ptr<Animation> DrawableElement::GetAnimationByName(std::string name) {}

std::shared_ptr<Animation> DrawableElement::GetAnimationFromState() {}

void DrawableElement::FlipAnimation(std::string name) {}

void DrawableElement::FlipAllAnimations() {}

bool DrawableElement::AllAnimationsFlipped() {}

std::string DrawableElement::GetType() {}

DrawableElement::~DrawableElement() {}