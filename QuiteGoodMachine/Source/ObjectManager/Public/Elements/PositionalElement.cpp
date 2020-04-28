#include "QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h"

#include "OpenGLIncludes.h"

#include <string>
#include <iostream>

/** CONSTRUCTORS **/
PositionalElement::PositionalElement()
   : ElementInterface("")
   , position_(glm::vec2(0.f, 0.f))
   , size_(glm::vec2(0.f, 0.f)) {}

PositionalElement::PositionalElement(std::string name, glm::vec2 initial_position, glm::vec2 size)
   : ElementInterface(name) 
{
   position_ = initial_position;
   size_ = size;
}

/** UPDATE **/
void PositionalElement::Update(bool freeze) {
   return;
}

/** GET AND SET POSITION **/
glm::vec2 PositionalElement::GetPosition() const {
   return position_;
}

void PositionalElement::SetPosition(glm::vec2 position) {
   position_ = position;
}

/** GET AND SET SIZE **/
glm::vec2 PositionalElement::GetSize() const {
   return size_;
}

void PositionalElement::SetSize(glm::vec2 size) {
   size_ = size;
}

/** GET TYPE **/
std::string PositionalElement::GetType() {
   return "PositionalElement";
}

/** DESTRUCTOR **/
PositionalElement::~PositionalElement() {}