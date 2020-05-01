#ifndef RENDERINGENGINE_H_
#define RENDERINGENGINE_H_

#include <glm/glm.hpp>

#include "QuiteGoodMachine/Source/RenderingEngine/Private/ShaderProgram.h"

#include "QuiteGoodMachine/Source/ObjectManager/Private/Element.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class Animation;

/* This class will be a singleton responsible for all rendering related
   stuff */
class RenderingEngine {
    public:
        /* Singleton instancing of this object 
           avoids making everything static */
        static RenderingEngine& GetInstance() {
            static RenderingEngine instance;
            return instance;
        }

        /* Load the screen settings */
        void LoadApplicationPerspective(std::string name, glm::mat4 projection);

        /* Load the shader */
        void LoadShader(std::string name);

        /* Get the shader by name */
        ShaderProgram *GetShaderReference(std::string name);

        /* Load resources for elements */
        void LoadResources(Element *element);
        void LoadResources(std::shared_ptr<Animation> animation);

        /* Load textures and return texture functions */
        Texture *LoadTexture(std::string name, const GLchar *file, GLboolean alpha = true);
        Texture LoadTextureFromFile(const GLchar *file, GLboolean alpha = true);
        Texture *GetTextureReference(std::string name);

        /* Keep track of shaders used */
        std::unordered_map<std::string, ShaderProgram> shaders;
        std::unordered_map<std::string, Texture> textures;

    private:
        /* Private constructor blocks any object from being made */
        RenderingEngine() {}
        ~RenderingEngine();

    public:
        /* Provides dynamic deletion in case of illegal reference */
        RenderingEngine(RenderingEngine const&) = delete;
        void operator=(RenderingEngine const&) = delete;

};

#endif