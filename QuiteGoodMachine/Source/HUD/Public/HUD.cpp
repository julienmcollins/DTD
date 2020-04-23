#include "QuiteGoodMachine/Source/HUD/Private/HUD.h"

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <algorithm>

#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

std::vector<std::shared_ptr<HUDElement>> HUD::GetHUDElements() {
   return HUD_element_list_;
}

std::shared_ptr<HUDElement> HUD::GetHUDElementByName(std::string name) {
   if (HUD_element_map_.count(name) > 0) {
      return HUD_element_map_[name];
   }
   std::cerr << "HUD::GetHUDElementByName - trying to access nonexistant HUD element" << std::endl;
   return nullptr;
}

void HUD::AddHUDElement(std::string name, std::shared_ptr<HUDElement> HUD_element) {
   // Add to map and list
   HUD_element_map_[name] = HUD_element;
   HUD_element_list_.push_back(HUD_element);
}

void HUD::RemoveHUDElement(std::string name) {
   // Find and remove from list
   HUD_element_list_.erase(std::find(HUD_element_list_.begin(), HUD_element_list_.end(), HUD_element_map_[name]));

   // Remove from map
   HUD_element_map_.erase(name);
}

void HUD::ClearHUDElementList() {
   // Clear the map and the list
   HUD_element_map_.clear();
   HUD_element_list_.clear();
}

void HUD::DisplayHUD() {
   for (int i = 0; i < HUD_element_list_.size(); i++) {
      HUD_element_list_[i]->Draw();
   }
}

void HUD::DisableHUD() {
   for (int i = 0; i < HUD_element_list_.size(); i++) {
      HUD_element_list_[i]->Disable();
   }
}

void HUD::EnableHUD() {
   for (int i = 0; i < HUD_element_list_.size(); i++) {
      HUD_element_list_[i]->Enable();
   }
}

void HUD::EnableHUDElement(std::string name) {
   if (HUD_element_map_.count(name) > 0) {
      HUD_element_map_[name]->Enable();
   }
}

void HUD::DisableHUDElement(std::string name) {
   if (HUD_element_map_.count(name) > 0) {
      HUD_element_map_[name]->Disable();
   }
}