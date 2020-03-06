#ifndef GLDATA_H_
#define GLDATA_H_

#include "OpenGLIncludes.h"

struct GLColorRGBA {
    float r;
    float g;
    float b;
    float a;
};

struct GLFloatRect {
    float x;
    float y;
    float w;
    float h;
};

struct GLVertexPos2D {
    float x;
    float y;
};

struct GLTexCoord {
    float s;
    float t;
};

struct GLTexRect {
    float t;
    float b;
    float l;
    float r;
};

struct GLTexturedVertex2D {
    GLVertexPos2D position;
	GLTexCoord tex_coord;
};

#endif