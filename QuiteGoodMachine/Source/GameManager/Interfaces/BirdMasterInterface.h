#ifndef BIRDMASTERINTERFACE_H_
#define BIRDMASTERINTERFACE_H_

#include <unordered_map>
#include <memory>

class Correspondence;
class Correspondent;

/** Router interface */
class BirdMasterInterface {
   public:
      /**
       * Add a correspondent to the list of active correspondents
       * 
       * @param PostalCode - post code of the added correspondent
       * @param Correspondent - the correspondent to be added
       */
      virtual void AddCorrespondent(const std::shared_ptr<Correspondent>& correspondent) = 0;

      /**
       * Removes a correspondent from the list of active correspondents
       * 
       * @param PostalCode - post code of the correspondent to be removed
       */
      virtual void RemoveCorrespondent(const std::shared_ptr<Correspondent>& correspondent) = 0;

      /**
       * Directs a message to appropriate senders based on Correspondence
       * 
       * @param Correspondence - the message and context to send
       */
      virtual void DirectMessage(std::shared_ptr<Correspondence> correspondence) = 0;

      /**
       * Add a subscription
       * 
       * @param Subscription - holds the subscription
       */
      // virtual void AddSubscription(std::shared_ptr<Subscription> subscription) = 0;

      /**
       * Remove a subscription
       * 
       * @param Subscription - holds the subscription to remove
       */
      // virtual void RemoveSubscription(std::shared_ptr<Subscription> subscription) = 0;

      /**
       * Virtual destructor
       */
      virtual ~BirdMasterInterface() {};
};

#endif