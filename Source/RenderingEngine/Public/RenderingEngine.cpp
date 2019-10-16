#include "Source/RenderingEngine/Private/RenderingEngine.h"

#include "Source/RenderingEngine/Private/ShaderProgram.h"

#include <string>
#include <map>

void RenderingEngine::LoadApplicationPerspective(glm::mat4 projection) {
    GetShader("texture_shader").Use().SetInteger("image", 0);
    GetShader("texture_shader").SetMatrix4("projection", projection);
}

void RenderingEngine::LoadShader(std::string name) {
   shaders.emplace(name, ShaderProgram());
   shaders[name].GetShaderFromFile("Shader/texture_shader.vs", "Shader/texture_shader.fs", nullptr);
}

ShaderProgram RenderingEngine::GetShader(std::string name) {
   return shaders[name];
}