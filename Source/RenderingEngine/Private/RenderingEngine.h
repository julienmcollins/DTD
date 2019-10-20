#ifndef RENDERINGENGINE_H_
#define RENDERINGENGINE_H_

#include <glm/glm.hpp>

#include "Source/RenderingEngine/Private/ShaderProgram.h"

#include "Source/ObjectManager/Private/Element.h"

#include <unordered_map>
#include <vector>
#include <string>

/* This class will be a singleton responsible for all rendering related
   stuff */

class RenderingEngine {
    public:
        /* Singleton instancing of this object 
           avoids making everything static */
        static RenderingEngine& get_instance() {
            static RenderingEngine instance;
            return instance;
        }

        /* Load the screen settings */
        void LoadApplicationPerspective(glm::mat4 projection);

        /* Load the shader */
        void LoadShader(std::string name);

        /* Get the shader by name */
        ShaderProgram GetShader(std::string name);

        /* Load resources for elements */
        bool LoadResources(Element *element, std::vector<TextureData> texture_data);
        void LoadResources(Element *element, std::unordered_map<std::string, Animation *> anim_data);

        /* Load the actual image for the element */
        void LoadImage(Element *element, int frame_num, float fps, std::string name, std::string file, bool &success, int rows = 1);

        /* Keep track of shaders used */
        std::unordered_map<std::string, ShaderProgram> shaders;

    private:
        /* Private constructor blocks any object from being made */
        RenderingEngine() {}

    public:
        /* Provides dynamic deletion in case of illegal reference */
        RenderingEngine(RenderingEngine const&) = delete;
        void operator=(RenderingEngine const&) = delete;

};

#endif