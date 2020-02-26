#version 330 core

// Attributes
layout (location = 0) in vec2 pos;

// Uniforms for matrices
uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = vec4(pos, 0.0, 1.0);
}