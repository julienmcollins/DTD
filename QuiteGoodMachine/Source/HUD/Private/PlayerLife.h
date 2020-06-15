#ifndef PLAYERLIFE_H_
#define PLAYERLIFE_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h"
#include "QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"

#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "OpenGLIncludes.h"

#include <memory>

class DrawStateContext;
class Texture;

class PlayerLife_Alive : public DrawState, 
                         public Correspondent {
   public:
      /**
       * Constructor
       * 
       * @param texture - texture related to the animation
       * @param animation - the animation itself
       */
      PlayerLife_Alive(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Pre transition
       */
      virtual void PreAction();

      /**
       * Process correspondence
       */
      virtual void ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence);

      /**
       * Reset function
       */
      virtual void Reset();

   private:
      // Flag for alive
      bool is_alive_;
};

class PlayerLife_Dead : public DrawState {
   public:
      /**
       * Constructor
       */
      PlayerLife_Dead(DrawStateContext *context, Texture *texture, std::shared_ptr<Animation> animation);

      /**
       * Animate function (stops at 18th frame)
       */
      virtual void Animate();
};

class PlayerLife : public HUDElement,
                   public Correspondent {
   public:
      /**
       * Constructor
       */
      PlayerLife(std::string name, glm::vec3 initial_position);

      /**
       * Load media function (should theoretically populate the state animations)
       */
      virtual void LoadMedia();

      /**
       * Process correspondence --> transitions alive to dead
       */
      virtual void ProcessCorrespondence(const std::shared_ptr<Correspondence>& correspondence);

      /**
       * Virtual reset function
       */
      virtual void Reset();
};

#endif