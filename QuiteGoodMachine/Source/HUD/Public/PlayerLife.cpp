#include "QuiteGoodMachine/Source/HUD/Private/PlayerLife.h"

#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

/** PLAYERLIFE **/
PlayerLife::PlayerLife(glm::vec3 initial_position)
   : HUDElement("PlayerLife", initial_position, glm::vec3(103.f, 76.f, 0.f)) {}

void PlayerLife::LoadMedia() {
   // Register texture first
   std::string hitmarker = Application::GetInstance().sprite_path + "Player/hitmarker_master_sheet.png";
   Texture *temp = RegisterTexture(hitmarker);

   // Register animations with state context
   GetStateContext()->RegisterState("alive", std::make_shared<PlayerLife_Alive>(temp, std::make_shared<Animation>(temp, "alive", 76.0f, 103.0f, 0.0f, 21, 1.0f / 20.0f)));
   GetStateContext()->RegisterState("dead", std::make_shared<PlayerLife_Dead>(temp, std::make_shared<Animation>(temp, "dead", 76.0f, 103.0f, 103.0f, 19, 1.0f / 20.0f)));
}

/** PLAYERLIFE_ALIVE **/
PlayerLife_Alive::PlayerLife_Alive(Texture *texture, std::shared_ptr<Animation> animation) 
   : DrawState(texture, animation) {}

void PlayerLife_Alive::PostTransition(StateContext *context) {}

/** PLAYERLIFE_DEAD **/
PlayerLife_Dead::PlayerLife_Dead(Texture *texture, std::shared_ptr<Animation> animation)
   : DrawState(texture, animation) {}