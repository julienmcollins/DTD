#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/TangibleElement.h"

#include <string>
#include <Box2D/Box2D.h>

#include "OpenGLIncludes.h"

TangibleElement::TangibleElement(std::string name, glm::vec3 initial_position, glm::vec3 size)
   : PositionalElement(name, initial_position, size) {}

void TangibleElement::Update(bool freeze) {}

void TangibleElement::SetCollision(uint16 collision_types, b2Fixture *fixtre) {}

void TangibleElement::SetHitbox(float x, float y, SHAPE_TYPE type, int group) {}

void TangibleElement::CreateHitbox(float x, float y) {}

void TangibleElement::Move() {}

uint8 TangibleElement::CheckCurrentContact() {}

void TangibleElement::SetCurrentContact(uint8 contact) {}

std::string TangibleElement::GetType() {}

TangibleElement::~TangibleElement() {}