#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <unordered_map>
#include <list>
#include <memory>

#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"
#include "QuiteGoodMachine/Source/GameManager/Interfaces/PigeonPostInterface.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/BirdMaster.h"

class PigeonPost : public PigeonPostInterface {
   public:
      /**
       * Instance of pigeonpost
       */
      static PigeonPost& GetInstance() {
         static PigeonPost instance;
         return instance;
      }

      /**
       * Register an object to the Pigeon Post
       */
      virtual void Register(std::string name, const std::shared_ptr<Correspondent>& correspondent);

      /**
       * Unregister a correspondent as a pigeon post
       */
      virtual void Unregister(std::string name, const std::shared_ptr<Correspondent>& correspondent);

      /**
       * Sends a correspondence to multiple receivers
       * 
       * @param correspondence The correspondence to send
       */
      virtual void Send(const Correspondence &correspondence);

      /**
       * Sends all correspondences
       */
      virtual void ProcessInbox();

      // Subscribing function allows elements to subscribe to different events
      // void SubscribeToEvent(Element *element, const Event &event);

      // Notify function will notify all relevant elements subscribed to a particular event
      // void NotifyAll(const Event &event);

   private:
      // Private constructor
      PigeonPost();

      // Bird master
      std::unique_ptr<BirdMaster> bird_master_;

   public:
      PigeonPost(PigeonPost const&) = delete;
      void operator=(PigeonPost const&) = delete;

      /**
       * Virtual destructor
       */
      virtual ~PigeonPost() {};
};

#endif