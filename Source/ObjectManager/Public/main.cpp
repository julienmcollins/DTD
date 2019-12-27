//
//  main.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include "Source/GameEngine/Private/Application.h"

#include <iostream>
#include <stdio.h>

int main(int argc, char * argv[]) {
    // Start up SDL and create window
    //Load media
    if (!Application::GetInstance().LoadMedia()) {
        printf("Failed to load media!\n");
    } else {
        //Apply the image
        Application::GetInstance().Update();
    }
    
    return 0;
}
