#include "ComponentHelpers.h"
#include "MeshComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponents.h"
#include "SkeletalNodeComponent.h"

#include <vector>
namespace CompHelpers
{

Float2 getSkeletalConnectionPoint(Entity &e, InternString connection)
{
   if(auto snc = e.getComponent<SkeletalNodeComponent>())
   if(snc->connections.find(connection) != snc->connections.end())
   if(auto gb = e.getComponent<GraphicalBoundsComponent>())
      return snc->connections[connection]->connectionPos * gb->size - (gb->center * gb->size);

   return Float2();
}

Rectf getEntityBounds(Entity& e)
{
   Rectf eBounds;
   if(auto pc = e.getComponent<PositionComponent>())
   {
      eBounds = Rectf(pc->pos.x, pc->pos.y, pc->pos.x, pc->pos.y);

      if(auto gb = e.getComponent<GraphicalBoundsComponent>())
      {
         eBounds = Rectf(
            pc->pos.x - (gb->size.x * gb->center.x),
            pc->pos.y - (gb->size.y * gb->center.y),
            pc->pos.x + (gb->size.x * (1.0f - gb->center.x)),
            pc->pos.y + (gb->size.y * (1.0f - gb->center.y))
         );
      }
   }

   return eBounds;
}

RenderLayer getRenderLayer(Entity &e)
{
   if(auto lc = e.getComponent<LayerComponent>())
      return lc->layer;

   return RenderLayer::Default;
}

void updateAnimationBind(Entity &e)
{
   if(auto bind = e.getComponent<BindAnimationComponent>())
   if(auto spr = e.getComponent<SpriteComponent>())   
   if(auto bindEntity = bind->entity.lock())
   if(auto bindSpr = bindEntity->getComponent<SpriteComponent>())
   {
      spr->dtMultiplier = bindSpr->dtMultiplier;
      spr->elapsedTime = bindSpr->elapsedTime;
      spr->face = bindSpr->face;
   }
}

void updatePositionBind(Entity &e)
{
   if(auto p = e.getComponent<PositionComponent>())
   if(auto bind = e.getComponent<PositionBindComponent>())
   if(auto bindEntity = bind->entity.lock())
   if(auto bindPos = bindEntity->getComponent<PositionComponent>())
   {
      p->pos = bindPos->pos + bind->offset;
      if(auto elev = bindEntity->getComponent<ElevationComponent>())
         p->pos.y -= elev->elevation;
   }
}

void addRectangleMeshComponent(Entity &e, Rectf rect, Colorf color)
{
   addRectangleMeshComponent(e, rect, color, color, color, color);
}

ICamera::Pass getRenderPass(Entity &e)
{
   if(auto c = e.getComponent<LightComponent>())
      return ICamera::Pass::Lighting;

   //return count to say default pass
   return ICamera::Pass::COUNT;
}

void addRectangleMeshComponent(Entity &e, Rectf rect, Colorf c1, Colorf c2, Colorf c3, Colorf c4)
{
   auto vertices = createVertexList()
      .with(VertexComponent::Position)
      .with(VertexComponent::Color)
      .with(VertexComponent::TextureCoordinate)
      .build();
   std::vector<int> indices;

   vertices.addVertex()
      .with<VertexComponent::Position>(Float2(rect.left, rect.top))
      .with<VertexComponent::Color>(c1)
      .with<VertexComponent::TextureCoordinate>(Float2(0, 1));

   vertices.addVertex()
      .with<VertexComponent::Position>(Float2(rect.right, rect.top))
      .with<VertexComponent::Color>(c2)
      .with<VertexComponent::TextureCoordinate>(Float2(1, 1));

   vertices.addVertex()
      .with<VertexComponent::Position>(Float2(rect.right, rect.bottom))
      .with<VertexComponent::Color>(c3)
      .with<VertexComponent::TextureCoordinate>(Float2(1, 0));

   vertices.addVertex()
      .with<VertexComponent::Position>(Float2(rect.left, rect.bottom))
      .with<VertexComponent::Color>(c4)
      .with<VertexComponent::TextureCoordinate>(Float2(0, 0));

   indices.push_back(0);indices.push_back(1);indices.push_back(3);
   indices.push_back(1);indices.push_back(3);indices.push_back(2);

   e.addComponent<MeshComponent>(std::make_shared<MeshComponent>(vertices, indices));
}
}