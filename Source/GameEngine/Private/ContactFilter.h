#include <Box2D/Box2D.h>
#include <string>
#include <iostream>

#include "Source/ObjecManager/Private/Element.h"
#include "Source/ObjecManager/Private/Object.h"

// Contact filter class
class ContactFilter : public b2ContactFilter {
    public:
        virtual bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
            bool result = true;
            void* bodyAUserData = fixtureA->GetBody()->GetUserData();
            void* bodyBUserData = fixtureB->GetBody()->GetUserData();
            if (bodyAUserData && bodyBUserData) {
                // Call callback
                Element *element1 = static_cast<Element*>(bodyAUserData);
                Element *element2 = static_cast<Element*>(bodyBUserData);

                // Start contacts
                element1->start_contact(element2);
                element2->start_contact(element1);
            }
            return result;
        }
};