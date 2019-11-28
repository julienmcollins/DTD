#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/ShaderProgram.h"
#include "Source/RenderingEngine/Private/Texture.h"

#include "Source/ObjectManager/Private/Element.h"

#include "Source/GameEngine/Private/Application.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

void RenderingEngine::LoadApplicationPerspective(std::string name, glm::mat4 projection) {
   GetShaderReference(name)->Use()->SetInteger("image", 0);
   GetShaderReference(name)->SetMatrix4("projection", projection);
}

void RenderingEngine::LoadShader(std::string name) {
   std::string vs_path = "Shader/" + name + ".vs";
   std::string fs_path = "Shader/" + name + ".fs";
   shaders[name] = ShaderProgram();
   shaders[name].GetShaderFromFile(vs_path.c_str(), fs_path.c_str(), nullptr);
}

ShaderProgram *RenderingEngine::GetShaderReference(std::string name) {
   if (shaders.find(name) != shaders.end()) {
      return &shaders[name];
   }
   return nullptr;
}

bool RenderingEngine::LoadResources(Element *element, std::vector<TextureData> texture_data) {
   // Success flag
   bool success = true;

   // Loop through and load
   for (auto const& data : texture_data) {
      LoadImage(element, data.num_of_frames, data.fps, data.name, data.path, success);   
   }

   // Return success
   return success;
}

void RenderingEngine::LoadResources(Element *element) {
   // Loop through and load
   for (std::unordered_map<std::string, Animation *>::iterator it = element->animations.begin(); it != element->animations.end(); ++it) {
      int i = 0;
      for (auto &frame : it->second->frames) {
         if (i == it->second->num_of_frames) {
            break;
         }
         frame.l = i * frame.normal_width;
         frame.r = (i + 1) * frame.normal_width;
         ++i;
      }
   }
}

void RenderingEngine::LoadImage(Element *element, int frame_num, float fps, std::string name, std::string file, bool &success, int rows) {
   element->textures.emplace(name, new Texture(element, frame_num - 1, fps));
   if (!element->textures[name]->LoadFromFile(file.c_str(), true)) {
      std::cerr << "Failed to load " << file << std::endl;
      success = false;
   } else {
      // Allocate space
      Texture *tex = element->textures[name];
      tex->clips_ = new GLFloatRect[frame_num];
      GLFloatRect *temp = tex->clips_;

      int columns = (frame_num + rows - 1) / rows;

      // Set sprites
      int k = 0;
      for (int i = 0; i < rows; i++) {
         for (int j = 0; j < columns; j++) {
            if (k == frame_num) {
               break;
            }
            // temp[k].x = (float) (j * tex->GetTextureWidth()) / (float) tex->GetImageWidth();
            // temp[k].y = (float) (i * tex->GetTextureHeight()) / (float) tex->GetImageHeight();
            // temp[k].w = (float) tex->GetTextureWidth() / (float) tex->GetImageWidth();
            // temp[k].h = (float) tex->GetTextureHeight() / (float) tex->GetImageHeight();
            k++;
         }
         if (k == frame_num) {
            break;
         }
      }

      // Set to 0
      element->textures[name]->curr_clip_ = &temp[0];
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