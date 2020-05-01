#ifndef TANGIBLENS_H_
#define TANGIBLENS_H_

namespace BODY {
   // Enum for directions
   enum DIRS {
      LEFT,
      RIGHT,
      NEUTRAL
   };

   // ENUM for contact type
   enum CONTACT {
      CONTACT_LEFT,
      CONTACT_RIGHT,
      CONTACT_UP,
      CONTACT_DOWN
   };

   enum COLLISIONS {
      CAT_PLAYER = 0x0001,
      CAT_PLATFORM = 0x0002,
      CAT_PROJECTILE = 0x0004,
      CAT_ENEMY = 0x0008,
      CAT_BOSS = 0x0010,
      CAT_SENSOR = 0x0020
   };

   enum ANTI_COLLISIONS {
      ACAT_PLAYER = 0xFFFE,
      ACAT_PLATFORM = 0xFFFD,
      ACAT_PROJECTILE = 0xFFFB,
      ACAT_ENEMY = 0xFFF7
   };
}

#endif