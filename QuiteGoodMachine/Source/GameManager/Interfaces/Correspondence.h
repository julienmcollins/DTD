#ifndef CORRESPONDENCE_H_
#define CORRESPONDENCE_H_

#include <string>

/**
 * Correspondence holds the actual message and/or event meant to be sent to the terminator
 */

class Correspondence {
   public:

   private:
      /** The message */
      void *message;

      /** The type of the message */
      std::string message_type;
};

#endif