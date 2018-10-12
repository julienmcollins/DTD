//
//  main.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/13/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "Application.h"
#undef main

int main(int argc, const char * argv[]) {
    // Create Application
    Application application;
    
    // Start up SDL and create window
    //Load media
    if (!application.loadMedia()) {
        printf("Failed to load media!\n");
    } else {
        //Apply the image
        application.update();
    }
    
    return 0;
}
