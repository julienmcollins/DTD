#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include "OpenGLIncludes.h"
#include <string>

class ShaderProgram {
    public:
        // Construct the shader program
        ShaderProgram();

        // Destructor for shader program
        virtual ~ShaderProgram();

        // Load the actual program
        virtual bool load_program() = 0;

        // Free the program
        virtual void free_program();

        // Bind the shaders to the program
        bool bind();

        // Unbind the program upon deletion
        void unbind();

        // Get the program id
        GLuint get_program_id();

    protected:
        // Print the program logs
        void print_program_log(GLuint program);

        // Print the shader log
        void print_shader_log(GLuint shader);

        // Load the shader from file
        GLuint load_shader_from_file(std::string path, GLenum shader_type);

        // Program ID
        GLuint program_ID;
};

#endif