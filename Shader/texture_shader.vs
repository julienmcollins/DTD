#version 330 core

// Attributes
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

// Tex coordinates
out vec2 tex_coord;

// Uniforms for matrices
uniform mat4 model;
uniform mat4 projection;

void main()
{
   tex_coord = tex;
	gl_Position = projection * model * vec4(pos, 0.0, 1.0);
}