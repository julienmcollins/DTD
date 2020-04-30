#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "OpenGLIncludes.h"

#include <string>
#include <iostream>

/** CONSTRUCTORS **/
PositionalElement::PositionalElement()
   : ElementInterface("")
   , position_(glm::vec3(0.f, 0.f, 0.f))
   , size_(glm::vec3(0.f, 0.f, 0.f)) {}

PositionalElement::PositionalElement(std::string name, glm::vec3 initial_position, glm::vec3 size, float angle)
   : ElementInterface(name)
   , position_(initial_position)
   , size_(size)
   , angle_(angle) {}

/** UPDATE **/
void PositionalElement::Update(bool freeze) {}

/** GET AND SET POSITION **/
glm::vec3 PositionalElement::GetPosition() const {
   return position_;
}

void PositionalElement::SetPosition(glm::vec3 position) {
   position_ = position;
}

/** GET AND SET SIZE **/
glm::vec3 PositionalElement::GetSize() const {
   return size_;
}

void PositionalElement::SetSize(glm::vec3 size) {
   size_ = size;
}

std::shared_ptr<StateContext> PositionalElement::GetStateContext() {
   return state_context_;
}

float PositionalElement::GetAngle() {
   return angle_;
}

/** GET TYPE **/
std::string PositionalElement::GetType() {
   return "PositionalElement";
}

/** DESTRUCTOR **/
PositionalElement::~PositionalElement() {}