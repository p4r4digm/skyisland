#pragma once

#include "Rect.h"
#include "Color.h"
#include "Entity.h"
#include "GraphicComponents.h"
#include "IViewport.h"

#include <string>

namespace CompHelpers
{
   void addRectangleMeshComponent(Entity &e, Rectf rect, Colorf color);
   void addRectangleMeshComponent(Entity &e, Rectf rect, Colorf c1, Colorf c2, Colorf c3, Colorf c4);
   Rectf getEntityBounds(Entity& e);
   void updatePositionBind(Entity &e);
   void updateAnimationBind(Entity &e);
   RenderLayer getRenderLayer(Entity &e);
   ICamera::Pass getRenderPass(Entity &e);
   Float2 getSkeletalConnectionPoint(Entity &e, InternString connection);
}

