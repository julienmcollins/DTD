 //
//  Texture.cpp
//  Janitor: The Story
//
//  Created by Julien Collins on 9/14/17.
//  Copyright © 2017 The Boys. All rights reserved.
//
#include <iostream>
#include "Texture.h"
#include "Global.h"
#include "Element.h"

// Texture constructor
Texture::Texture(Element *element, int max_frame, float fps_val) : clips_(NULL), curr_clip_(NULL), 
   frame_(0), completed_(false), max_frame_(max_frame), fps(fps_val), last_frame(0.0f),
   reset_frame(0), stop_frame(max_frame), flip_(SDL_FLIP_NONE), has_flipped_(false), angle(0.0f), element_(element), 
   m_texture(NULL), m_width(0), m_height(0), x(0), y(0) {
   
   // Set x and y positions if element isn't null
      /*
   if (element) {
      set_x(element->get_x());
      set_y(element->get_y());
   }
   */

   // Start timer
   fps_timer.start();   
}

// Loads textures from files
bool Texture::loadFromFile(std::string path) {
    // Free preexisting textures
    free();
    
    // The final texture
    SDL_Texture* newTexture = NULL;
    
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        } else {
            // Get image dimensions
            m_width = loadedSurface->w;
            m_height = loadedSurface->h;
            center_ = {m_width / 2, m_height / 2};
        }
        
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    
    // Return success
    m_texture = newTexture;
    return m_texture != NULL;
}

// Free textures
void Texture::free() {
    // Free texture if it exists
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
        m_width = 0;
        m_height = 0;
    }
}

// Render textures
void Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, m_width, m_height};
    
    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    // Render to screen
    SDL_RenderCopyEx(renderer, m_texture, clip, &renderQuad, angle, center, flip);

    // Set x and y position
    this->x = x;
    this->y = y;
}

// Gets image width
int Texture::getWidth() const {
    return m_width;
}

// Gets image height
int Texture::getHeight() const {
    return m_height;
}

// Get x position
int Texture::get_x() const {
    return x;
}

// Get y position
int Texture::get_y() const {
    return y;
}

Texture &Texture::operator=(const Texture &src) {
   // Set render equal to it
   // renderer = src.renderer;
   
   // Set textures equal to eachother
   m_texture = src.m_texture;
   
   // Set dimensions equal to image dimension
   m_width = src.m_width;
   m_height = src.m_height;

   // Add a bunch of other missing variables
   max_frame_ = src.max_frame_;
   clips_ = new SDL_Rect[max_frame_ + 1];
   for (int i = 0; i < max_frame_ + 1; i++) {
      clips_[i] = src.clips_[i];
   }
   
   // Return
   return (*this);
}

// Destructor calls free
Texture::~Texture() {
   //free();
}
