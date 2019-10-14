//
//  main.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include "Source/Private/Application.h"

#include <iostream>
#include <stdio.h>

int main(int argc, char * argv[]) {
    // Start up SDL and create window
    //Load media
    if (!Application::get_instance().loadMedia()) {
        printf("Failed to load media!\n");
    } else {
        //Apply the image
        Application::get_instance().update();
    }
    
    return 0;
}
