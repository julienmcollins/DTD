#ifndef BIRDMASTER_H_
#define BIRDMASTER_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/BirdMasterInterface.h"

#include <unordered_map>
#include <queue>

class BirdMaster : public BirdMasterInterface {
   public:
      /**
       * Constructor
       */
      BirdMaster() {};

      /**
       * Add a correspondent to the list of active correspondents
       * 
       * @param PostalCode - post code of the added correspondent
       * @param Correspondent - the correspondent to be added
       */
      virtual void AddCorrespondent(const std::shared_ptr<Correspondent>& correspondent);

      /**
       * Removes a correspondent from the list of active correspondents
       * 
       * @param PostalCode - post code of the correspondent to be removed
       */
      virtual void RemoveCorrespondent(const std::shared_ptr<Correspondent>& correspondent);

      /**
       * Directs a message to appropriate senders based on Correspondence
       * 
       * @param Correspondence - the message and context to send
       */
      virtual void DirectCorrespondence(std::shared_ptr<Correspondence> correspondence);

      /**
       * Enqueues a message
       * 
       * @param correspondence - enqueues a message
       */
      virtual void Enqueue(std::shared_ptr<Correspondence> correspondence);

      /**
       * Process the queue (send out messages)
       */
      virtual void ProcessQueue();

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
      virtual ~BirdMaster() {};

   private:
      /** Active recipients - map of address to correspondents */
      std::unordered_map<int, std::shared_ptr<Correspondent>> active_correspondents_;

      // Queue for messages
      std::queue<std::shared_ptr<Correspondence>> correspondence_queue_;
};

#endif