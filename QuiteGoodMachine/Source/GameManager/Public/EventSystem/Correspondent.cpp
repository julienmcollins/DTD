#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"
#include "QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h"

#include <memory>

Correspondent::Correspondent() {}

void Correspondent::RegisterAsCorrespondent(std::string name) {
   // Create guid
   postal_code_ = ObjectManager::GetInstance().GenerateGlobalID(name);
}

void Correspondent::UnregisterAsCorrespondent() {
   // Delete global id and association
}

// void Correspondent::RegisterAsReceiver() {
//    is_receiver_ = true;
// }

// void Correspondent::RegisterAsSender() {
//    is_sender_ = true;
// }

// void Correspondent::UnregisterAsReceiver() {
//    is_receiver_ = false;
// }

// void Correspondent::UnregisterAsSender() {
//    is_sender_ = false;
// }

void Correspondent::ReceiveCorrespondence(const std::shared_ptr<Correspondence>& correspondence) {
   // Call delegate function for processing messages (TODO)
   ProcessCorrespondence(correspondence);
}