#include "Source/RenderingEngine/Private/ShaderProgram.h"

#include "Source/GameEngine/Private/Application.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Instantiate program id to null
ShaderProgram::ShaderProgram() {
	// Set program id to 0 at beginning
	program_ID = 0;
}

// Delete the program
ShaderProgram::~ShaderProgram() {
    // Free the program
    std::cout << "Deleting program\n";
    glDeleteProgram(program_ID);
}

// Bind the shaders to the program
ShaderProgram *ShaderProgram::Use() {
   // Use the shader
   glUseProgram(program_ID);
	return this;
}

void ShaderProgram::GetShaderFromFile(const GLchar *vertex_shader_file, const GLchar *frag_shader_file, const GLchar *geo_shader_file) {
	// Define file paths
	string vertex_code;
	string frag_code;
	string geo_code;

	// Attempt to load
	try {
		// Open the files
		ifstream vertex_file(vertex_shader_file);
		ifstream frag_file(frag_shader_file);
		stringstream vertex_stream, frag_stream;

		// Read the file's content
		vertex_stream << vertex_file.rdbuf();
		frag_stream << frag_file.rdbuf();

		// Close the handlers
		vertex_file.close();
		frag_file.close();

		// Convert the stream into string
		vertex_code = vertex_stream.str();
		frag_code = frag_stream.str();

		// Load geo shader if present
		if (geo_shader_file) {
			ifstream geo_file(geo_shader_file);
			stringstream geo_stream;
			geo_stream << geo_file.rdbuf();
			geo_file.close();
			geo_code = geo_stream.str();
		}
	} catch (exception e) {
		cout << "ShaderProgram::GetShaderFromFile() - Error reading shader files\n";
	}

	// Convert to cstrings
	const GLchar *vertex_shader_code = vertex_code.c_str();
	const GLchar *frag_shader_code = frag_code.c_str();
	const GLchar *geo_shader_code = geo_shader_file != nullptr ? geo_code.c_str() : nullptr;

	// Load the shader
	LoadShaderProgram(vertex_shader_code, frag_shader_code, geo_shader_code);
}

void ShaderProgram::LoadShaderProgram(const GLchar *vertex_source, const GLchar *frag_source, const GLchar *geo_source) {
	// Set use program to 0?
   glUseProgram(0);

   // Define shaders
	GLuint vertex, frag, geo;

	// Load the vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_source, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	// Load fragment shader
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_source, NULL);
	glCompileShader(frag);
	CheckCompileErrors(frag, "FRAGMENT");

	// Load geo shader (if it exists)
	if (geo_source) {
		std::cout << "jere\n";
		geo = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geo, 1, &geo_source, NULL);
		glCompileShader(geo);
		CheckCompileErrors(geo, "GEOMETRY");
	}

	// Create the actual program
	program_ID = glCreateProgram();
	glAttachShader(program_ID, vertex);
	glAttachShader(program_ID, frag);
	if (geo_source) {
		glAttachShader(program_ID, geo);
	}

	// Link the program
	glLinkProgram(program_ID);
	CheckCompileErrors(program_ID, "PROGRAM");

	// Delete the shaders
	glDeleteShader(vertex);
	glDeleteShader(frag);
	if (geo_source) {
		glDeleteShader(geo);
	}
}

void ShaderProgram::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(program_ID, name), value);
}
void ShaderProgram::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(program_ID, name), value);
}
void ShaderProgram::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(program_ID, name), x, y);
}
void ShaderProgram::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(program_ID, name), value.x, value.y);
}
void ShaderProgram::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(program_ID, name), x, y, z);
}
void ShaderProgram::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(program_ID, name), value.x, value.y, value.z);
}
void ShaderProgram::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(program_ID, name), x, y, z, w);
}
void ShaderProgram::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(program_ID, name), value.x, value.y, value.z, value.w);
}
void ShaderProgram::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
   if (useShader)
      this->Use();
   glUniformMatrix4fv(glGetUniformLocation(program_ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::CheckCompileErrors(GLuint object, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}