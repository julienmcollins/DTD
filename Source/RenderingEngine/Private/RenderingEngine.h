#ifndef RENDERINGENGINE_H_
#define RENDERINGENGINE_H_

#include <string>
#include <map>

#include <glm/glm.hpp>

#include "Source/RenderingEngine/Private/ShaderProgram.h"

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

        /* Keep track of shaders used */
        std::map<std::string, ShaderProgram> shaders;

    private:
        /* Private constructor blocks any object from being made */
        RenderingEngine() {}

    public:
        /* Provides dynamic deletion in case of illegal reference */
        RenderingEngine(RenderingEngine const&) = delete;
        void operator=(RenderingEngine const&) = delete;

};

#endif