#include "QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/ShaderProgram.h"
#include "QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"

#include "QuiteGoodMachine/Source/GameManager/Private/Application.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

void RenderingEngine::LoadApplicationPerspective(std::string name, glm::mat4 projection) {
   GetShaderReference(name)->Use()->SetInteger("image", 0);
   GetShaderReference(name)->SetMatrix4("projection", projection);
}

void RenderingEngine::LoadShader(std::string name) {
   std::string vs_path = "QuiteGoodMachine/Shader/" + name + ".vs";
   std::string fs_path = "QuiteGoodMachine/Shader/" + name + ".fs";
   shaders[name] = ShaderProgram();
   shaders[name].GetShaderFromFile(vs_path.c_str(), fs_path.c_str(), nullptr);
}

ShaderProgram *RenderingEngine::GetShaderReference(std::string name) {
   if (shaders.find(name) != shaders.end()) {
      return &shaders[name];
   }
   return nullptr;
}

void RenderingEngine::LoadResources(Element *element) {
   // Loop through and load
   for (std::unordered_map<std::string, Animation *>::iterator it = element->animations.begin(); it != element->animations.end(); ++it) {
      int i = 0;
      int j = 0;
      for (auto &frame : it->second->frames) {
         if (i == it->second->num_of_frames) {
            break;
         }

         // Automatically set the columns
         frame.l = i * frame.normal_width;
         frame.r = (i + 1) * frame.normal_width;
         ++i;

         // Automatically set the rows
         if (it->second->rows > 1) {
            int mod = it->second->columns > 0 ? it->second->columns : (it->second->num_of_frames + (it->second->rows - 1)) / it->second->rows;
            frame.b = j * frame.normal_height;
            frame.t = (j + 1) * frame.normal_height;
            if (i % mod == 0) {
               j++;
            }
         }
      }
   }
}

Texture *RenderingEngine::LoadTexture(std::string name, const GLchar *file, GLboolean alpha) {
   // Check if exists
   if (textures.find(name) == textures.end()) {
      textures[name] = LoadTextureFromFile(file, alpha);
   }
   return &textures[name];
}

Texture RenderingEngine::LoadTextureFromFile(const GLchar *file, GLboolean alpha) {
   // Create the texture object
   Texture texture;

   // Check alpha
   if (alpha) {
      texture.internal_format = GL_RGBA;
      texture.image_format = GL_RGBA;
   } else {
      texture.internal_format = GL_RGB;
      texture.image_format = GL_RGB;
   }

   // Load the image
   texture.LoadFromFile(file, alpha);

   // Return the texture
   return texture;
}

Texture *RenderingEngine::GetTextureReference(std::string name) {
   if (textures.find(name) != textures.end()) {
      return &textures[name];
   }
   return nullptr;
}

RenderingEngine::~RenderingEngine() {
   // Go through and free every texture
   for (std::unordered_map<std::string, Texture>::iterator it = textures.begin(); it != textures.end(); ++it) {
      it->second.Free();
   }
}