#include <GLFW/glfw3.h>

#include "RenderManager.h"
#include "PositionComponent.h"
#include "MeshComponent.h"
#include "Renderables.h"

#include "Renderer.h"
#include "ComponentHelpers.h"
#include "SeanSort.h"
#include "PhysicsComponents.h"
#include "TextComponent.h"
#include "GraphicComponents.h"
#include "SkeletalNodeComponent.h"

RenderManager::RenderManager()
{
   m_renderer = std::move(createRenderer());
   m_renderThread.reset(new RenderThread());
   m_renderThread->start();
}

RenderManager::~RenderManager()
{
   
}

void RenderManager::terminate()
{
   m_renderThread->stop();
}

bool RenderManager::renderViewport(IViewport &vp)
{
   auto &camera = vp.getCamera();
   auto &scene = camera->getScene();

   if(!m_renderer->newScene(vp, *camera))
      return false;

   auto _eList = scene->getEntities(camera->getBounds());
   std::vector<std::shared_ptr<Entity>> eList;

   //some predraw component operations
   for(auto ent : _eList)
   {
      CompHelpers::updatePositionBind(*ent);
      CompHelpers::updateAnimationBind(*ent);

      //skip entity if it has a parent and that parent still exists
      if(auto rpc = ent->getComponent<RenderParentComponent>())
         if(rpc->parent.lock())
            continue;

      //generate new entity list for drawing
      eList.push_back(ent);
   }

   //sort the list by Y-position
   eList = seanSort(std::move(eList), [&](const std::shared_ptr<Entity> &e1, const std::shared_ptr<Entity> &e2)->bool
   {
      auto bot1 = e1->partitionBounds.bottom;
      auto bot2 = e2->partitionBounds.bottom;

      if(bot1 < bot2) 
         return true;
      if(bot1 > bot2) 
         return false;
      
      return e1 < e2;
   });

   //and render
   for(auto ent : eList)
      renderEntity(*ent);

   return true;
}

void RenderManager::renderEntity(Entity &entity)
{
   auto childrenComp = entity.getComponent<RenderChildrenComponent>();

   //draw bg children
   if(childrenComp)
   {
      for(int i = 0; i < childrenComp->parentIndex; ++i)
         if(auto e = childrenComp->children[i].lock())
            renderEntity(*e);         
   }

   if(auto mc = entity.getComponent<MeshComponent>())
      buildMeshRenderable(entity)->render(*m_renderer); 

   if(auto tc = entity.getComponent<TextComponent>())
      buildTextRenderable(entity)->render(*m_renderer);

   if(auto snc = entity.getComponent<SkeletalNodeComponent>())
   {
      buildSkeletalRenderable(entity)->render(*m_renderer);
   }

   if(childrenComp)
   {
      for(int i = childrenComp->parentIndex; i < childrenComp->children.size(); ++i)
         if(auto e = childrenComp->children[i].lock())
            renderEntity(*e);   
   }
      
}

void RenderManager::finalizeRender()
{
   m_renderThread->resetQueue(std::move(m_renderer->drawQueue()));     

}

