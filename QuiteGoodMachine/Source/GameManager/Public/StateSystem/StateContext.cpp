#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"
#include "QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include <memory>
#include <iostream>

StateContext::StateContext(PositionalElement *base) 
   : base_(base) {}

PositionalElement *StateContext::GetBase() const {
   return base_;
}

void StateContext::SetState(std::shared_ptr<StateInterface> state) {
   current_state_ = state;
}

std::shared_ptr<StateInterface> StateContext::GetCurrentState() {
   return current_state_;
}

std::shared_ptr<StateInterface> StateContext::GetState(std::string name) {
   if (registered_states_.count(name) > 0) {
      return registered_states_[name];
   }
   std::cout << "StateContext::GetState - attempting to get non-registered state" << std::endl;
   return GetCurrentState();
}

void StateContext::RegisterState(std::string name, std::shared_ptr<StateInterface> new_state) {
   // Insert only if unique within set
   if (registered_states_.find(name) != registered_states_.end()) {
      return;
   }
   // registered_states_[name] = new_state;
   auto p = std::make_pair(name, new_state);
   registered_states_.insert(p);
}

void StateContext::DoAction() {
   current_state_->DoAction(this);
}

void StateContext::SetResetState(std::shared_ptr<StateInterface> reset_state) {
   reset_state_ = reset_state;
}

void StateContext::Reset() {
   current_state_ = reset_state_;
   current_state_->Reset();
}