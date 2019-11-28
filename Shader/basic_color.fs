#version 330 core

// Output fragment
out vec4 frag;

// texture sampler
uniform vec3 color;

void main()
{
	frag = vec4(color, 1.0);
}