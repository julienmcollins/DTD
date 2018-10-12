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

class Texture {
    public:
        // Initialize
        Texture();
        
        // Load image at specified path
        bool loadFromFile(std::string path);
        
        // Deallocate texture
        void free();
        
        // Render texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
        // The renderer for the textures
        // SDL_Renderer* renderer;
    
        // Get image dimensions
        int getWidth() const;
        int getHeight() const;

        // Get image positions
        int get_x() const;
        int get_y() const;
    
        // Assignement operator
        Texture &operator= (const Texture &src);
    
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
