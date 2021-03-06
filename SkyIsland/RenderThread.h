#pragma once

#include "IRenderer.h"
#include "TextureManager.h"
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

class RenderThread
{
   std::mutex m_mutex;
   std::thread m_thread;
   IRenderer::SceneListPtr m_queueFilling, m_queueCurrent;
   bool m_queueUpdated;
   std::atomic_bool m_isRunning;

   std::shared_ptr<TextureManager> m_texManager;

   void _run();
public:

   RenderThread();

   void start();
   void stop();

   void resetQueue(IRenderer::SceneListPtr queue);

};