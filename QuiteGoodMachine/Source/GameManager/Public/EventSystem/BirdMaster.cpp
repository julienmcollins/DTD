#include <unordered_map>
#include <iostream>

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/BirdMaster.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"

#include "QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h"

void BirdMaster::AddCorrespondent(const std::shared_ptr<Correspondent>& correspondent) {
   // Add to active correspondents
   int cguid = correspondent->GetPostalCode();
   active_correspondents_.insert(std::pair<int, std::shared_ptr<Correspondent>>(cguid, correspondent));
}

void BirdMaster::RemoveCorrespondent(const std::shared_ptr<Correspondent>& correspondent) {
   // Remove correspondent if found
   int cguid = correspondent->GetPostalCode();
   if (active_correspondents_.count(cguid) == 0) {
      return;
   }
   active_correspondents_.erase(cguid);
}

void BirdMaster::DirectCorrespondence(std::shared_ptr<Correspondence> correspondence) {
   // Get a list of recipients
   std::vector<int> recipients = correspondence->GetCorrespondents();
   std::vector<std::shared_ptr<Correspondent>> terminators;

   // Iterate and send messages to them
   for (auto& rec : recipients) {
      std::shared_ptr<Correspondent> correspondent = active_correspondents_[rec];
      terminators.push_back(correspondent);
   }

   // Receive messages
   for (auto& c : terminators) {
      c->ReceiveCorrespondence(correspondence);
   }
}

void BirdMaster::Forward(std::string name, const std::shared_ptr<Correspondence>& correspondence) {
   int rec = ObjectManager::GetInstance().GetUID(name);
   if (active_correspondents_.count(rec) > 0) {
      active_correspondents_[rec]->ReceiveCorrespondence(correspondence);
   }
}

void BirdMaster::Enqueue(std::shared_ptr<Correspondence> correspondence) {
   correspondence_queue_.push(correspondence);
}

void BirdMaster::ProcessQueue() {
   while (!correspondence_queue_.empty()) {
      DirectCorrespondence(correspondence_queue_.front());
      correspondence_queue_.pop();
   }
}