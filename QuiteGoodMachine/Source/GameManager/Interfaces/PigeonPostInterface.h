#ifndef PIGEONPOST_INTERFACE_H_
#define PIGEONPOST_INTERFACE_H_

#include <vector>
#include <memory>

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"
#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h"

/** Bus interface */
class PigeonPostInterface {
   public:
      /**
       * Register an object to the Pigeon Post
       */
      virtual void Register(const std::shared_ptr<Correspondent>& correspondent) = 0;
      
      /**
       * Unregister a correspondent as a pigeon post
       */
      virtual void Unregister(const std::shared_ptr<Correspondent>& correspondent) = 0;

      /**
       * Sends a correspondence to multiple receivers
       * 
       * @param correspondence The correspondence to send
       */
      virtual void Send(const Correspondence &correspondence) = 0;

      /**
       * Virtual destructor
       */
      virtual ~PigeonPostInterface() {};
};

#endif