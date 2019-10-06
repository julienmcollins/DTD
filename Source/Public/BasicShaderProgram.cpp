#include "Source/Private/BasicShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

BasicShaderProgram::BasicShaderProgram() {
    vertex_pos2D_location_ = 0;
	tex_coord_location_ = 0;
    tex_coord_location_ = 0;

	tex_unit_location_ = 0;

    projection_matrix_location_ = 0;
    modelview_matrix_location_ = 0;
}

bool BasicShaderProgram::load_program() {
    //Generate program
	program_ID = glCreateProgram();

	//Load vertex shader
	GLuint vertexShader = load_shader_from_file( "Shader/BasicShaderProgram.glvs", GL_VERTEX_SHADER );

    //Check for errors
    if( vertexShader == 0 )
    {
        glDeleteProgram( program_ID );
        program_ID = 0;
        return false;
    }

	//Attach vertex shader to program
	glAttachShader( program_ID, vertexShader );

	//Create fragment shader
	GLuint fragmentShader = load_shader_from_file( "Shader/BasicShaderProgram.glfs", GL_FRAGMENT_SHADER );

    //Check for errors
    if( fragmentShader == 0 )
    {
        glDeleteShader( vertexShader );
        glDeleteProgram( program_ID );
        program_ID = 0;
        return false;
    }

	//Attach fragment shader to program
	glAttachShader( program_ID, fragmentShader );

	//Link program
    glLinkProgram( program_ID );

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv( program_ID, GL_LINK_STATUS, &programSuccess );
	if (programSuccess != GL_TRUE) {
		printf( "Error linking program %d!\n", program_ID );
		print_program_log( program_ID );
        glDeleteShader( vertexShader );
        glDeleteShader( fragmentShader );
        glDeleteProgram( program_ID );
        program_ID = 0;
        return false;
    }

	//Clean up excess shader references
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

	//Get variable locations
	vertex_pos2D_location_ = glGetAttribLocation( program_ID, "vertex_pos_2D" );
	if( vertex_pos2D_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "vertex_pos_2D" );
	}

	tex_coord_location_ = glGetAttribLocation( program_ID, "tex_coord" );
	if( tex_coord_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "tex_coord" );
	}

	color_location_ = glGetUniformLocation( program_ID, "tex_color" );
	if( color_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "tex_color" );
	}

	tex_unit_location_ = glGetUniformLocation( program_ID, "tex_unit" );
	if( tex_unit_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "tex_unit" );
	}

	projection_matrix_location_ = glGetUniformLocation( program_ID, "projection_matrix" );
	if( projection_matrix_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "projection_matrix" );
	}

	modelview_matrix_location_ = glGetUniformLocation( program_ID, "modelview_matrix" );
	if( modelview_matrix_location_ == -1 )
	{
		printf( "%s is not a valid glsl program variable!\n", "modelview_matrix" );
	}

	return true;
}

void BasicShaderProgram::set_vertex_pointer(GLsizei stride, const GLvoid *data) {
    glVertexAttribPointer( vertex_pos2D_location_, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void BasicShaderProgram::set_tex_pointer(GLsizei stride, const GLvoid *data) {
    glVertexAttribPointer( tex_coord_location_, 2, GL_FLOAT, GL_FALSE, stride, data );
}

void BasicShaderProgram::enable_data_pointers() {
    glEnableVertexAttribArray( vertex_pos2D_location_ );
	glEnableVertexAttribArray( tex_coord_location_ );
}

void BasicShaderProgram::disable_data_pointers() {
    glDisableVertexAttribArray( vertex_pos2D_location_ );
	glDisableVertexAttribArray( tex_coord_location_ );
}

void BasicShaderProgram::set_projection_matrix(glm::mat4 matrix) {
    projection_matrix_ = matrix;
}

void BasicShaderProgram::set_modelview_matrix(glm::mat4 matrix) {
    modelview_matrix_ = matrix;
}

void BasicShaderProgram::left_mult_projection(glm::mat4 matrix) {
    projection_matrix_ = matrix * projection_matrix_;
}

void BasicShaderProgram::left_mult_modelview(glm::mat4 matrix) {
    modelview_matrix_ = matrix * modelview_matrix_;
}

void BasicShaderProgram::update_projection_matrix() {
    glUniformMatrix4fv(	projection_matrix_location_, 1, GL_FALSE, glm::value_ptr( projection_matrix_ ) );
}

void BasicShaderProgram::update_modelview_matrix() {
    glUniformMatrix4fv(	modelview_matrix_location_, 1, GL_FALSE, glm::value_ptr( modelview_matrix_ ) );
}

void BasicShaderProgram::set_texture_color(ColorRGBA color) {
	glUniform4f( color_location_, color.r, color.g, color.b, color.a );
}

void BasicShaderProgram::set_texture_unit(GLuint unit) {
	glUniform1i( tex_unit_location_, unit );
}