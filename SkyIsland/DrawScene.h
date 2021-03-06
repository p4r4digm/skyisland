#pragma once

#include "ICamera.h"
#include "IViewport.h"
#include "IDrawObject.h"
#include "GraphicComponents.h"
#include "FBO.h"

#include <vector>
#include <memory>
#include "GnUtilities.h"
#include <map>

typedef std::vector<std::shared_ptr<IDrawObject>> DrawQueue;

struct DrawPass
{
   std::unique_ptr<IDrawObject> fboTexture;
   std::vector<DrawQueue> drawQueue;
   std::shared_ptr<FBO> fbo;
   DrawPass()
   {
      drawQueue.resize((int)RenderLayer::COUNT);
   }

   DrawPass(DrawPass && ref):
      fboTexture(std::move(ref.fboTexture)), fbo(std::move(fbo)), drawQueue(std::move(ref.drawQueue))
   {}

   DrawPass &operator=(DrawPass && ref)
   {
      fbo = std::move(ref.fbo);
      fboTexture = std::move(ref.fboTexture);
      drawQueue = std::move(ref.drawQueue);
      return *this;
   }
};

class DrawScene
{
private:
   std::vector<DrawQueue> m_drawQueue;
   std::map<ICamera::Pass, std::unique_ptr<DrawPass>> m_passes;
   Rectf m_vpBounds;
   Rectf m_camBounds;
   Rectf m_scissorBounds;
   bool m_scissor;

   void renderObjectList(std::vector<DrawQueue> &queues);
   void renderFBOObjectList(ICamera::Pass pass, DrawPass &dp);
   std::unique_ptr<IDrawObject> buildFboDrawObject(std::shared_ptr<FBO> fbo);

public:
   DrawScene(IViewport &vp, ICamera &camera);
   DrawScene(IViewport &vp, ICamera &camera, Rectf scissorBounds);

   void addObjectToPass(ICamera::Pass pass, RenderLayer layer, std::shared_ptr<IDrawObject> obj);
   void addObject(RenderLayer layer, std::shared_ptr<IDrawObject> obj);
   void draw();


};
