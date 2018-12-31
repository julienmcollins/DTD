#include <iostream>
#include "Object.h"
#include "Texture.h"

/******************** Object Implementations **********************/

// Constructor
Object::Object(int x, int y, double height, double width, Application *application) :
   Element(x, y, height, width, application) {}

/***************** Platform Implementations *************************/

// Constructor
Platform::Platform(int x, int y, Application *application) :
   Object(x, y, texture_.getHeight(), texture_.getWidth(), application) {}

// doNothing
//void Platform::doNothing() {}

/************** Projectile Implementations  *************************/

// Constructor
Projectile::Projectile(int x, int y, bool owner, bool damage, Application *application) :
   Object(x, y, texture_.getHeight(), texture_.getWidth(), application), 
   owner_(owner), damage_(damage) {}

// Do nothing function
//void Projectile::doNothing() {}

// Get owner of projectile
bool Projectile::get_owner() const {
   return owner_;
}

// Get damage of projectile
int Projectile::get_damage() const {
   return damage_;
}
