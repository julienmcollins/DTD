#ifndef CORRESPONDENTINTERFACE_H_
#define CORRESPONDENTINTERFACE_H_

#include <memory>

class Correspondence;

/**
 * Correpspondent interface allows anyone who implements this to receive a correspondence 
 */
class CorrespondentInterface {
   public:
      /**
       *  Register as a correspondent
       */
      virtual void RegisterAsCorrespondent() = 0;

      /** 
       * Unregister as a correspondent
       */
      virtual void UnregisterAsCorrespondent() = 0;

      /**
       * Register as a receiver
       */
      virtual void RegisterAsReceiver() = 0;

      /**
       * Register as a sender
       */
      virtual void RegisterAsSender() = 0;

      /**
       * Register as a receiver
       */
      virtual void UnregisterAsReceiver() = 0;

      /**
       * Register as a sender
       */
      virtual void UnregisterAsSender() = 0;

      /**
       * Receives the correspondence from message buss
       * 
       * @param correspondence The correspondence received
       */
      virtual void ReceiveCorrespondence(const std::shared_ptr<Correspondence>& correspondence) = 0;

      /**
       * Virtual destructor
       */
      virtual ~CorrespondentInterface() {};
};

#endif