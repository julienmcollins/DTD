#include "Source/Private/ShaderProgram.h"

#include <fstream>
#include <iostream>

using namespace std;

// Instantiate program id to null
ShaderProgram::ShaderProgram() {
    program_ID = NULL;
}

// Delete the program
ShaderProgram::~ShaderProgram() {
    // Free the program
    free_program();
}

// Free the program
void ShaderProgram::free_program() {
    glDeleteProgram(program_ID);
}

// Bind the shaders to the program
bool ShaderProgram::bind() {
    // Use the shader
    glUseProgram(program_ID);

    // Check for error
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "Error binding shader! " << gluErrorString(error) << endl;
        print_program_log(program_ID);
        return false;
    }

    return true;
}

// Unbind the shader by binding it to NULL
void ShaderProgram::unbind() {
    glUseProgram(NULL);
}

// Get the program id back
GLuint ShaderProgram::get_program_id() {
    return program_ID;
}

// Print the program log
void ShaderProgram::print_program_log(GLuint program) {
    //Make sure name is shader
	if( glIsProgram( program ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
            cout << infoLog << std::endl;
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
        cout << "Name " << program << " is not a program\n";
	}
}

// Print shader log
void ShaderProgram::print_shader_log(GLuint shader) {
    //Make sure name is shader
	if( glIsShader( shader ) )
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		char* infoLog = new char[ maxLength ];

		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
            cout << infoLog << endl;
		}

		//Deallocate string
		delete[] infoLog;
	}
	else
	{
        cout << "Name " << shader << " is not a shader\n";
	}
}

// Load the shader from file
GLuint ShaderProgram::load_shader_from_file(std::string path, GLenum shader_type) {
    // Open file
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile( path.c_str() );

	//Source file loaded
	if(sourceFile) {
	    //Get shader source
		shaderString.assign( ( std::istreambuf_iterator< char >( sourceFile ) ), std::istreambuf_iterator< char >() );

		//Create shader ID
		shaderID = glCreateShader( shader_type );

        //Set shader source
        const GLchar* shaderSource = shaderString.c_str();
        glShaderSource( shaderID, 1, (const GLchar**)&shaderSource, NULL );

        //Compile shader source
        glCompileShader( shaderID );

        //Check shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled );
        if( shaderCompiled != GL_TRUE )
        {
            cout << "Unable to compile shader " << shaderID << "\n\nSource:\n" << shaderSource << "\n";
            print_shader_log( shaderID );
            glDeleteShader( shaderID );
            shaderID = 0;
        }
	} else {
        cout << "Unable to open file " << path.c_str() << endl;
    }

	return shaderID;
}