#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"

Correspondence::Correspondence(void *message, std::string message_type, std::vector<int>& correspondents, int originator) {
   message_ = message;
   message_type_ = message_type;
   correspondents_ = correspondents;
   originator_ = originator;
}

void *Correspondence::GetMessage() const {
   return message_;
}

std::vector<int> Correspondence::GetCorrespondents() const {
   return correspondents_;
}

int Correspondence::GetOriginator() const {
   return originator_;
}

Correspondence Correspondence::CompileCorrespondence(void *message, std::string message_type, std::vector<int>& correspondents, int originator) {
   Correspondence correspondence(message, message_type, correspondents, originator);
   return correspondence;
}