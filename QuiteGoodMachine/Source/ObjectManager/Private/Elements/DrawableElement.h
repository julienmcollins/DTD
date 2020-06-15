#ifndef DRAWABLEELEMENT_H_
#define DRAWABLEELEMENT_H_

#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h"

#include "OpenGLIncludes.h"

#include <string>
#include <memory>

class DrawableElement : virtual public PositionalElement {
   public:
      /**
       * Constructor
       */
      DrawableElement(std::string name, glm::vec3 initial_position, glm::vec3 size, float angle = 0.f, glm::mat4 *parent = nullptr);

      // Friend class texture
      friend class Texture;
      friend class DrawState;

      /**
       * Registers the texture by file path
       */
      Texture *RegisterTexture(std::string file);

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
      virtual void Draw();

      /**
       * Animates the element
       */
      virtual void Animate();

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
       * Get state context?
       */
      std::shared_ptr<DrawStateContext> GetStateContext();

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
       * Set draw_model_
       */
      void SetDrawModel(glm::mat4 m);

      /**
       * GetType function returns the type this element
       */
      virtual std::string GetType();
   
      /**
       * Virtual destructor
       */
      virtual ~DrawableElement();

   protected:
      // Position vector
      glm::vec3 draw_position_;

      // Main texture and extra textures
      // std::shared_ptr<Texture> main_texture_;
      Texture *main_texture_;
      std::unordered_map<std::string, Texture*> additional_textures_;

      // Animation map
      std::unordered_map<std::string, std::shared_ptr<Animation>> animation_map_;

      // Drawing model and angle
      glm::mat4 draw_model_;
      float draw_angle_;
};

#endif