#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/ObjectManager.h"

#include <memory>

Correspondent::Correspondent(std::shared_ptr<Element> element) {
   element_ = element;
}

void Correspondent::RegisterAsCorrespondent() {
   // Create guid
   postal_code_ = ObjectManager::GetInstance().GenerateGlobalID(this);

   // Register with bus
   PigeonPost::GetInstance().Register(getptr());
}

void Correspondent::UnregisterAsCorrespondent() {
   // Delete global id and association
   
   // Unregister with bus
   PigeonPost::GetInstance().Unregister(getptr());
}

void Correspondent::RegisterAsReceiver() {
   is_receiver_ = true;
}

void Correspondent::RegisterAsSender() {
   is_sender_ = true;
}

void Correspondent::UnregisterAsReceiver() {
   is_receiver_ = false;
}

void Correspondent::UnregisterAsSender() {
   is_sender_ = false;
}

void Correspondent::ReceiveCorrespondence(const std::shared_ptr<Correspondence>& correspondence) {
   // Call delegate function for processing messages (TODO)
   ProcessCorrespondence();
}