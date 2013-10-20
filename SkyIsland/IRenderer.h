#pragma once

#include <vector>
#include <memory>
#include "Vertex.h"
#include "IDrawObject.h"
#include "Transform.h"
#include "DrawScene.h"
#include "StringTable.h"

class IRenderer
{
public:
   typedef std::unique_ptr<DrawScene> DScenePtr;
   typedef std::unique_ptr<std::vector<DScenePtr>> SceneList;
   
   virtual ~IRenderer(){}

   virtual void drawTriangles(std::shared_ptr<VertexList> vertices, std::shared_ptr<std::vector<int>> faces, Transform transform = Transform()) const=0;
   virtual void drawTexture(InternString texture, std::shared_ptr<VertexList> vertices, std::shared_ptr<std::vector<int>> faces, Transform transform = Transform()) const=0;

   virtual SceneList drawQueue()=0;
   virtual bool newScene(IViewport &vp, ICamera &cam)=0; //returns whether new scene will be drawn

};