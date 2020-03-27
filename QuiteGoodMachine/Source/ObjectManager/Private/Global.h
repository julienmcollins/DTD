//
//  Global.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/19/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Global_h
#define Global_h

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h"

extern SDL_Renderer* renderer;
extern std::string sprite_path;
extern std::string player_path;
extern std::string enemy_path;

#endif /* Global_h */
