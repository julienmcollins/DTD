#include "QuiteGoodMachine/Source/HUD/Private/PlayerLife.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include <string>

/** PLAYERLIFE **/
PlayerLife::PlayerLife(std::string name, glm::vec3 initial_position)
   : HUDElement(name, initial_position, glm::vec3(103.f, 76.f, 0.f))
   , PositionalElement(name, initial_position, glm::vec3(103.f, 76.f, 0.f))
{
   Disable();
}

void PlayerLife::LoadMedia() {
   // TODO: Figure out why Application::GetInstance() doesn't work
   // Register texture first
   std::string hitmarker = "Media/Sprites/Player/hitmarker_master_sheet.png";
   Texture *temp = RegisterTexture(hitmarker);

   // Register animations with state context
   GetStateContext()->RegisterState("alive", std::make_shared<PlayerLife_Alive>(temp, std::make_shared<Animation>(temp, "alive", 76.0f, 103.0f, 0.0f, 21, 1.0f / 20.0f)));
   GetStateContext()->RegisterState("dead", std::make_shared<PlayerLife_Dead>(temp, std::make_shared<Animation>(temp, "dead", 76.0f, 103.0f, 103.0f, 19, 1.0f / 20.0f)));

   // Register as correspondent (and state)
   RegisterAsCorrespondent(GetName());
   PlayerLife_Alive *alive_state = static_cast<PlayerLife_Alive*>(GetStateContext()->GetState("alive").get());
   alive_state->RegisterAsCorrespondent(GetName() + "_Alive");

   // Set current state
   GetStateContext()->SetState(GetStateContext()->GetState("alive"));
}

void PlayerLife::ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence) {
   char *msg = (char *) (correspondence->GetMessage());
   std::string fmsg(msg);
   if (fmsg == "LifeLost") {
      PigeonPost::GetInstance().Forward(GetName() + "_Alive", correspondence);
   }
}

/** PLAYERLIFE_ALIVE **/
PlayerLife_Alive::PlayerLife_Alive(Texture *texture, std::shared_ptr<Animation> animation) 
   : DrawState(texture, animation)
   , is_alive_(true) {}

void PlayerLife_Alive::PreTransition(StateContext *context) {
   if (!is_alive_) {
      context->SetState(context->GetState("dead"));
   }
}

void PlayerLife_Alive::ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence) {
   // Check for dead msg
   char *msg = (char *) (correspondence->GetMessage());
   std::string fmsg(msg);
   if (fmsg == "LifeLost") {
      is_alive_ = false;
   }
}

/** PLAYERLIFE_DEAD **/
PlayerLife_Dead::PlayerLife_Dead(Texture *texture, std::shared_ptr<Animation> animation)
   : DrawState(texture, animation) {}