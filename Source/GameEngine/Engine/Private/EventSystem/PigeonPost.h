#ifndef EVENT_H_
#define EVENT_H_

#include "Source/GameEngine/ObjectManager/Private/Element.h"

#include <string>
#include <unordered_map>
#include <list>

class PigeonPost {
   public:
      static PigeonPost& GetInstance() {
         static PigeonPost instance;
         return instance;
      }

      // Subscribing function allows elements to subscribe to different events
      // void SubscribeToEvent(Element *element, const Event &event);

      // Notify function will notify all relevant elements subscribed to a particular event
      // void NotifyAll(const Event &event);

   private:
      // Private constructor
      PigeonPost();

      // Private map of lists (subscribers)
      std::unordered_map<std::string, std::list<Element*>> subscribers_;

   public:
      PigeonPost(PigeonPost const&) = delete;
      void operator=(PigeonPost const&) = delete;
};

#endif