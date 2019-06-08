//
//  Texture.h
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Timer.h"

class Element;

typedef struct {
   int height;
   int width;
   int frames;
} TextureData;

class Texture {
    public:
        // Initialize
        Texture(Element *element = NULL, int max_frame = 0, float fps_val = 0);

        // Copy constructor
        //Texture(const Texture &tex);
        
        // Load image at specified path
        bool loadFromFile(std::string path);
        
        // Deallocate texture
        void free();
        
        // Render texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
        // Rect for sprite
        SDL_Rect *clips_;
        SDL_Rect *curr_clip_;

        // Frame related variables
        int frame_;
        int max_frame_;
        float fps;
        float last_frame;
        Timer fps_timer;

        // Start and stop frames
        int reset_frame;
        int stop_frame;

        // Flag for completed animation
        bool completed_;

        // Flip parameter
        SDL_RendererFlip flip_;
        bool has_flipped_;

        // Center parameter
        SDL_Point center_;

        // Get image dimensions
        int getWidth() const;
        int getHeight() const;

        // Set image positions
        void set_x(int new_x) {
           x = new_x;
        }
        void set_y(int new_y) {
           y = new_y;
        }

        // Add to image positions
        void add_x(int new_x) {
           x += new_x;
        }
        void add_y(int new_y) {
           y += new_y;
        }

        // Get image positions
        int get_x() const;
        int get_y() const;

        // Element referring to the texture
        Element *element_;
    
        // Assignement operator
        Texture &operator= (const Texture &src);

        // Destructor
        ~Texture();
    
    private:
        // Hardware texture
        SDL_Texture* m_texture;
        
        // Image dimensions
        int m_width;
        int m_height;

        // Image position
        int x;
        int y;
};

#endif /* Texture_h */
