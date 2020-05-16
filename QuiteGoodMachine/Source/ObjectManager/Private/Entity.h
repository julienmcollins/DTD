#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <vector>

#include "QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h"
#include "QuiteGoodMachine/Source/GameManager/Private/ClassType.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h"
#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/TangibleElement.h"

#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"

#define PC_OFF_X 62.0f
#define PC_OFF_Y 0.0f
#define SIM_GRAV -3.0f

class Platform;
class Projectile;
class Player;
class Animation;

class Entity : public DrawableElement
             , public TangibleElement {
   public:
      // Constructor
      Entity(std::string name, glm::vec3 initial_position, glm::vec3 size);

      // Movement and updating
      virtual void Update(bool freeze = false);

      // Get type
      virtual std::string GetType() {
         return "Entity";
      }

      // Create projectile (might need to add an entity pointer just in case)
      // virtual Projectile* CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
      //       bool owner, bool damage, float force_x, float force_y);

      // Destructor
      virtual ~Entity();

   private:
      // Health amount
      int health_;

      // Flag to determine if alive
      bool alive_;

   // Setters and getters
   public:
      /**
       * Get health
       */
      int GetHealth() const {
         return health_;
      }

      /**
       * Set health
       */
      void SetHealth(int health) {
         health_ = health;
      }

      /**
       * Check if alive
       */
      bool IsAlive() const {
         return alive_;
      }

      /**
       * Set is alive
       */
      void SetIsAlive(bool alive) {
         alive_ = alive;
      }
};

/************** PLAYER BODY PARTS ********************/
class PlayerHead : public BodyPart {
   public:
      PlayerHead(Player *player, float x_rel, float y_rel);
      
      // Start contact function
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return "PlayerHead";
      }
};

class PlayerArm : public BodyPart {
   public:
      PlayerArm(Player *player, float x_rel, float y_rel, int width, int height, std::string type);

      // Start contact function
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }

   private:
      std::string type_;
};

class PlayerHand : public BodyPart {
   public:
      PlayerHand(Player *player, float x_rel, float y_rel, std::string type);

      // Start contact function
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }

   private:
      std::string type_;
};

class PlayerLeg : public BodyPart {
   public:
      PlayerLeg(Player *player, float x_rel, float y_rel, int width, int height, std::string type);

      // Start contact function
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      virtual std::string type() {
         return "Player";
      }

      std::string sub_type() {
         return type_;
      }
   
   private:
      std::string type_;
};

// Player class
class Player : public Entity, 
               public Correspondent {
   public:
      // Construct the player
      Player();

      // Contact enums
      enum CONTACTS {
         HEAD = 0,
         LEFT_ARM = 1,
         RIGHT_ARM = 2,
         LEFT_HAND = 3,
         RIGHT_HAND = 4,
         LEFT_LEG = 5,
         RIGHT_LEG = 6
      };

      // Flags
      int has_jumped_;

      // Shooting flag
      bool shooting;

      /******** OPENGL ***********/
      // Sheet for arms
      Texture *arm_sheet;
      Texture *projectile_sheet;
      /***************************/

      // Arm delta displacement
      int arm_delta_x;
      int arm_delta_y;
      int arm_delta_shoot_x;
      int arm_delta_shoot_y;

      // Adjust delta function
      void adjust_deltas();

      // Update function now done in player
      virtual void Update(bool freeze = false);
      virtual void Move();

      // Contact listener
      virtual void StartContact(Element *element = NULL);
      virtual void EndContact(Element *element = NULL);

      // Damage function
      void TakeDamage(int damage);

      // Get type
      virtual std::string type() {
         return "Player";
      }

      // Load media function for the player
      virtual void LoadMedia();

      // Create projectile
      // virtual Projectile* CreateProjectile(std::string name, float width, float height, int delta_x_r, int delta_x_l, int delta_y, 
      //       bool owner, bool damage, float force_x, float force_y);

      // Flags for interactions
      bool contacts_[5];

      // Virtual destructor
      virtual ~Player();
   private:
      // Has an arm
      DrawableElement arm_;
      
      // Player body parts
      std::vector<BodyPart*> player_body_right_;
      std::vector<BodyPart*> player_body_left_;

      // Deactivation flags
      bool right_deactivated_ = false;
      bool left_deactivated_ = true;

      // Immunity timer
      FPSTimer immunity_timer_;
      float immunity_duration_;

      // Edge timer
      FPSTimer edge_timer_;
      float edge_duration_;

      // Fall timer
      FPSTimer fall_timer_;
      float fall_duration_;

      // Projectile pointer
      ClassType<Projectile> eraser;
      Projectile *eraser;
      int num_of_projectiles;
};

#endif
