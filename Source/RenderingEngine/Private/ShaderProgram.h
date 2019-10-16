#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include "OpenGLIncludes.h"
#include <string>

class ShaderProgram {
    public:
        /* Constructor will take in the specific shaders and construct them */
        ShaderProgram();

        /* Virtual destructor for now */
        virtual ~ShaderProgram();
        
        /* Program ID of the shader */
        GLuint program_ID;

        /* Use the shader */
        ShaderProgram &Use();

        /* Load's the shader program with the vertex and frag shaders */
        void GetShaderFromFile(const GLchar *vertex_shader_file, const GLchar *frag_shader_file, const GLchar *geo_shader_file);

        /* Load's the shader program */
        void LoadShaderProgram(const GLchar *vertex_source, const GLchar *frag_source, const GLchar *geo_source);
    
        /* Uniform utility functions */
        void SetFloat    (const GLchar *name, GLfloat value, GLboolean useShader = false);
        void SetInteger  (const GLchar *name, GLint value, GLboolean useShader = false);
        void SetVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
        void SetVector2f (const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
        void SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
        void SetVector3f (const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
        void SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
        void SetVector4f (const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
        void SetMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

    private:
        // Checks if compilation or linking failed and if so, print the error logs
        void CheckCompileErrors(GLuint object, std::string type);
};

#endif