#ifndef PLAYERLIFE_H_
#define PLAYERLIFE_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"

#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "OpenGLIncludes.h"

#include <memory>

class StateContext;
class Texture;

class PlayerLife_Alive : public DrawState, public Correspondent {
   public:
      /**
       * Constructor
       * 
       * @param texture - texture related to the animation
       * @param animation - the animation itself
       */
      PlayerLife_Alive(Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pre transition
       */
      virtual void PreTransition(StateContext *context);

      /**
       * Process correspondence
       */
      virtual void ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence);

   private:
      // Flag for alive
      bool is_alive_;
};

class PlayerLife_Dead : public DrawState, 
                        public Correspondent {
   public:
      /**
       * Constructor
       */
      PlayerLife_Dead(Texture *texture, std::shared_ptr<Animation> animation);
};

class PlayerLife : public HUDElement {
   public:
      /**
       * Constructor
       */
      PlayerLife(std::string name, glm::vec3 initial_position);

      /**
       * Load media function (should theoretically populate the state animations)
       */
      virtual void LoadMedia();
};

#endif