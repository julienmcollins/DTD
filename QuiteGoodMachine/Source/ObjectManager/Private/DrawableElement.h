#ifndef DRAWABLEELEMENT_H_
#define DRAWABLEELEMENT_H_

#include "QuiteGoodMachine/Source/ObjectManager/Interfaces/ElementInterface.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "QuiteGoodMachine/Source/GameManager/Private/State.h"

#include <string>
#include <memory>

class DrawableElement : virtual public ElementInterface {
   public:
      /**
       * Constructor
       */
      DrawableElement();

      /**
       * Update function will involve drawing if enabled
       * TODO: Remove freeze
       * TODO: Determine if this class should be abstract still
       */
      virtual void Update(bool freeze = false);

      /**
       * Load's the media related to the element
       */
      virtual void LoadMedia();

      /**
       * Draws the element
       */
      void Draw();

      /**
       * Animates the element
       */
      void Animate();

      /**
       * Get reference to animation by its name (if it exists)
       * 
       * @return shrd_ptr - Shared ptr reference to animation
       */
      std::shared_ptr<Animation> GetAnimationByName(std::string name);

      /**
       * Get reference to animation based on it's current state
       * TODO: Check if need to be virtual
       */
      virtual std::shared_ptr<Animation> GetAnimationFromState();

      /**
       * Flips specified animation
       */
      void FlipAnimation(std::string name);

      /**
       * Flips all animations
       */
      void FlipAllAnimations();

      /**
       * Checks if all animations are flipped
       */
      bool AllAnimationsFlipped();

      /**
       * GetType function returns the type this element
       */
      virtual std::string GetType();
   
      /**
       * Virtual destructor
       */
      virtual ~DrawableElement();

   private:
      // Position vector
      glm::vec2 draw_position_;

      // Main texture and extra textures
      std::shared_ptr<Texture> main_texture_;
      std::unordered_map<std::string, std::shared_ptr<Texture>> additional_textures_;

      // Animation map
      std::unordered_map<std::string, std::shared_ptr<Animation>> animation_map_;

      // Reference to current state
      std::shared_ptr<State> current_state_;

      // Drawing model and angle
      glm::mat4 model_;
      float angle_;
};

#endif