#version 330 core

// Output fragment
out vec4 frag;

// Input tex coord
in vec2 tex_coord;

// texture sampler
uniform sampler2D image;
uniform vec3 color;

void main()
{
	frag = vec4(color, 1.0) * texture(image, tex_coord);
}