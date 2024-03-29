#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

#include <string>

HUDElement::HUDElement(std::string name, glm::vec3 initial_position, glm::vec3 size, bool start_enabled) 
   : DrawableElement(name, initial_position, size)
   , PositionalElement(name, initial_position, size)
   , is_enabled_(start_enabled)
   , is_interactable_(false) {}

void HUDElement::Update(bool freeze) {
   if (IsEnabled()) {
      GetStateContext()->DoAction();
   }
}

void HUDElement::Enable() {
   is_enabled_ = true;
}

void HUDElement::Disable() {
   is_enabled_ = false;
}

bool HUDElement::IsEnabled() {
   return is_enabled_;
}

void HUDElement::MakeInteractable() {
   is_interactable_ = true;
}

void HUDElement::MakeNonInteractable() {
   is_interactable_ = false;
}

bool HUDElement::IsInteractable() {
   return is_interactable_;
}

void HUDElement::Reset() {
   GetStateContext()->Reset();
}