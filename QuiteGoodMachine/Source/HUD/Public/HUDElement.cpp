#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

#include <string>

HUDElement::HUDElement(std::string name, glm::vec2 initial_position, glm::vec2 size) 
   : DrawableElement(name, initial_position, size) {}

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