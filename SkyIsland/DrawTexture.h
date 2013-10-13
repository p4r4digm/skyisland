#pragma once

#include "IDrawObject.h"

#include <vector>
#include "Vertex.h"
#include "Transform.h"

class DrawTexture : public IDrawObject
{
   std::vector<Vertex>  m_vertices;
   std::vector<int> m_faces;

   Transform m_transform;

   std::string m_texture;

public:
   DrawTexture(std::string texture, std::vector<Vertex> vertices, std::vector<int> faces, Transform transform=Transform());

   void draw();

};