#ifndef CORRESPONDENT_H_
#define CORRESPONDENT_H_

#include "QuiteGoodMachine/Source/GameManager/Interfaces/CorrespondentInterface.h"

class Element;

class Correspondent : public CorrespondentInterface, std::enable_shared_from_this<Correspondent> {
   public:
      /**
       * Constructor
       * 
       * @param element A shared pointer to the underlying element
       */
      Correspondent(std::shared_ptr<Element> element);

      /**
       * Get the postal code
       * 
       * @return int Returns the postal code of the correspondent
       */
      int GetPostalCode() const {
         return postal_code_;
      }

      /** 
       * Register as a correspondent 
       */
      virtual void RegisterAsCorrespondent();

      /** 
       * Unregister as a correspondent
       */
      virtual void UnregisterAsCorrespondent();

      /**
       * Register as a receiver
       */
      virtual void RegisterAsReceiver();

      /**
       * Register as a sender
       */
      virtual void RegisterAsSender();

      /**
       * Register as a receiver
       */
      virtual void UnregisterAsReceiver();

      /**
       * Register as a sender
       */
      virtual void UnregisterAsSender();
      
      /**
       * Check if a registered receiver
       * 
       * @return Whether the correspondent is a receiver
       */
      bool IsRegisteredAsReceiver() {
         return is_receiver_;
      }

      /**
       * Check if a registered sender
       * 
       * @return Whether the correspondent is a sender
       */
      bool IsRegisteredAsSender() {
         return is_sender_;
      }
      
      /**
       * Receives the correspondence from message buss
       * 
       * @param correspondence The correspondence received
       */
      virtual void ReceiveCorrespondence(const std::shared_ptr<Correspondence>& correspondence);

      /**
       * Process the message received
       */
      virtual void ProcessCorrespondence() {};

      /**
       * Get shared ptr from this
       */
      std::shared_ptr<Correspondent> getptr() {
         return shared_from_this();
      }

      /**
       * Virtual destructor
       */
      virtual ~Correspondent() {};
      
   private:
      /** Postal code of the correspondent */
      int postal_code_;

      /** Handler for the element */
      std::shared_ptr<Element> element_;

      /** Booleans for receiver and sender */
      bool is_receiver_;
      bool is_sender_;
};

#endif