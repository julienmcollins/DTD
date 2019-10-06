#ifndef GLDATA_H_
#define GLDATA_H_

#include "OpenGLIncludes.h"

struct ColorRGBA
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

struct FloatRect
{
    GLfloat x;
    GLfloat y;
    GLfloat w;
    GLfloat h;
};

struct VertexPos2D
{
    GLfloat x;
    GLfloat y;
};

struct TexCoord
{
    GLfloat s;
    GLfloat t;
};

struct LTexturedVertex2D
{
    VertexPos2D position;
	TexCoord texCoord;
};

#endif