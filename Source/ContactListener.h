#include <Box2D/Box2D.h>
#include <string>
#include <iostream>
#include "Element.h"
#include "Object.h"

// Contact Listener Class
class ContactListener : public b2ContactListener {
   public:
      // Contact listener for beginning of contact
      void BeginContact(b2Contact* contact) {
         //check if fixture A and B are touching
         void* bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData();
         void* bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData();
         if (bodyAUserData && bodyBUserData) {
           static_cast<Element*>(bodyAUserData)->start_contact();
           static_cast<Element*>(bodyBUserData)->start_contact();
         }
      }

      // Contact listener for end of contact
      void EndContact(b2Contact* contact) {
         //check if fixture A was a ball
         void* bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData();
         void* bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData();
         if ( bodyAUserData ) {
           static_cast<Element*>( bodyAUserData )->end_contact();
         }
      }
};