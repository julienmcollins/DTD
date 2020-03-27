#ifndef CORRESPONDENCE_H_
#define CORRESPONDENCE_H_

#include <string>
#include <vector>

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h"

/**
 * Correspondence holds the actual message and/or event meant to be sent to the terminator
 */

class Correspondence {
   public:
      /**
       * Default constructor
       */
      Correspondence() {};

      /**
       * Initializing constructor
       * 
       * @param message The message
       * @param correspondents The list of correspondants
       * @param originator The originator of the message
       */
      Correspondence(void *message, std::string message_type, std::vector<int>& correspondents, int originator);

      /**
       * Get the message
       * 
       * @return returns a void * message
       */
      void *GetMessage() const;

      /**
       * Return the list of correspondents
       * 
       * @return returns a vector of postal codes
       */
      std::vector<int> GetCorrespondents() const;

      /**
       * Return the originator of the correspondence
       * 
       * @return Correspondent The originator of the correspondence
       */
      int GetOriginator() const;

      /**
       * Creates a correspondence - static function
       * 
       * @param message The message related to the correspondence
       * @param correspondents A list of receivers of the message
       * @param originator The correspondent who sent the message
       * @return Returns a correspondence with the info
       */
      static Correspondence CompileCorrespondence(void *message, std::string message_type, std::vector<int> &correspondents, int originator);

   private:
      /** The message */
      void *message_;

      /** The type of the message */
      std::string message_type_;

      /** Relevant correspondents */
      std::vector<int> correspondents_;

      /** Originator of the correspondence */
      int originator_;
};

#endif