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
         void* bodyAUserData = contact->GetFixtureA()->GetUserData();
         void* bodyBUserData = contact->GetFixtureB()->GetUserData();
         if (bodyAUserData && bodyBUserData) {
            // Create dummy pointers
            Element *element1 = static_cast<Element*>(bodyAUserData);
            Element *element2 = static_cast<Element*>(bodyBUserData);

            // Start contacts
            element1->start_contact(element2);
            element2->start_contact(element1);
         }
      }

      // Contact listener for end of contact
      void EndContact(b2Contact* contact) {
         //check if fixture A was a ball
         void* bodyAUserData = contact->GetFixtureA()->GetUserData();
         void* bodyBUserData = contact->GetFixtureB()->GetUserData();
         if ( bodyAUserData ) {
            Element *element1 = static_cast<Element*>( bodyAUserData );
            Element *element2 = static_cast<Element*>( bodyBUserData );

            // End contacts
            element1->end_contact();
            element2->end_contact();
         }
      }
};
