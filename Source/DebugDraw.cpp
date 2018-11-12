#include <iostream>
#include "DebugDraw.h"
#include "Application.h"
#include "Global.h"

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
   SDL_Rect poly;
   poly.x = vertices[0].x * 100.0f;
   poly.y = -vertices[0].y * 100.0f;
   poly.w = (vertices[0].x - vertices[1].x) > 0 ? (vertices[0].x - vertices[1].x) * 100.0f : (vertices[1].x - vertices[0].x) * 100.0f;
   poly.h = (vertices[0].y - vertices[1].y) > 0 ? (vertices[0].y - vertices[1].y) * 100.0f : (vertices[1].y - vertices[0].y) * 100.0f;
   application_->r = poly;
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
   SDL_Rect poly;
   poly.x = vertices[0].x * 100.0f;
   poly.y = -vertices[0].y * 100.0f;
   poly.w = (vertices[0].x - vertices[1].x) > 0 ? (vertices[0].x - vertices[1].x) * 100.0f : (vertices[1].x - vertices[0].x) * 100.0f;
   poly.h = (vertices[0].y - vertices[1].y) > 0 ? (vertices[0].y - vertices[1].y) * 100.0f : (vertices[1].y - vertices[0].y) * 100.0f;
   SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
   SDL_RenderFillRect(renderer, &poly);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
   bool a;
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
   bool a;
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
   bool a;
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
   bool a;
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
   bool a;
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
   bool a;
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
   std::cout << "Reached this point" << std::endl;
   SDL_Rect poly;
   poly.x = aabb->upperBound.x;
   poly.y = aabb->upperBound.y;
   poly.w = aabb->GetExtents().x * 2;
   poly.h = aabb->GetExtents().y * 2;
   SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
   SDL_RenderFillRect(renderer, &poly);
}