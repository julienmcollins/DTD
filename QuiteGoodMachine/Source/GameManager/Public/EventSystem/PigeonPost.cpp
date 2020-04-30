#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"

#include <iostream>

PigeonPost::PigeonPost() {
   // Create the bird master
   bird_master_ = std::make_unique<BirdMaster>();
}

void PigeonPost::Register(std::string name, const std::shared_ptr<Correspondent>& correspondent) {
   // Register as correspondent
   correspondent->RegisterAsCorrespondent(name);

   // Relay to bird_master_
   bird_master_->AddCorrespondent(correspondent);
}

void PigeonPost::Unregister(std::string name, const std::shared_ptr<Correspondent>& correspondent) {
   // Relay to bird_master_
   bird_master_->RemoveCorrespondent(correspondent);
}

void PigeonPost::Send(const Correspondence &correspondence) {
   // Relay to bird_master_
   // bird_master_->DirectCorrespondence(std::make_shared<Correspondence>(correspondence));
   bird_master_->Enqueue(std::make_shared<Correspondence>(correspondence));
}

void PigeonPost::ProcessInbox() {
   bird_master_->ProcessQueue();
}