#include "CharacterController.h"
#include "PhysicsComponents.h"
#include "GraphicComponents.h"
#include "Animations.h"
#include "IOCContainer.h"
#include "Application.h"
#include "StringTable.h"

CharacterController::CharacterController(std::weak_ptr<Entity> entity):
   m_entity(std::move(entity))
{
   m_taskDone = false;
   m_accel = 0.5f;
   m_runAccel = 1.0f;
   m_maxVelocity = 10.0f;
   m_friction = 0.5f;
   m_minAnimSpeed = 0.09f;
   m_maxAnimSpeed = 0.5f;

   setState(buildMoveState());
}

StatePtr CharacterController::buildMoveState()
{
   class MoveState : public CharacterState
   {
      CharacterController &cc;

   public:
      MoveState(CharacterController &cc):cc(cc){}

      void move(Float2 vector)
      {
         if(vector != Float2()) 
            cc.m_facing = vector;

         if(auto e = cc.m_entity.lock())
         {
            if(auto ac = e->getComponent<AccelerationComponent>())
            if(auto fc = e->getComponent<FrictionComponent>())
            {
               fc->friction = 0.0f;
               ac->direction = vector;
               ac->acceleration = cc.m_accel;
               ac->maxVelocity = cc.m_maxVelocity;
            }
         }   
      }

      void stop()
      {
         if(auto e = cc.m_entity.lock())
         {
            if(auto ac = e->getComponent<AccelerationComponent>())
            if(auto fc = e->getComponent<FrictionComponent>())
            {
               fc->friction = cc.m_friction;
               ac->acceleration = 0.0f;
            }
         }
      }

      void updateAnimation()
      {
         auto st = IOC.resolve<StringTable>();

         if(auto e = cc.m_entity.lock())
         {
            if(auto ac = e->getComponent<AccelerationComponent>())
            if(auto fc = e->getComponent<FrictionComponent>())
            if(auto vc = e->getComponent<VelocityComponent>())
            if(auto spr = e->getComponent<SpriteComponent>())
            {
               if(vc->velocity == Float2())
               {
                  //stopped, set face
                  if(fabs(cc.m_facing.x) > fabs(cc.m_facing.y))
                     spr->face = cc.m_facing.x >= 0.0f ? st->get("stand_right") : st->get("stand_left");
                  else
                     spr->face = cc.m_facing.y >= 0.0f ? st->get("stand_up") : st->get("stand_down");
               }
               else
               {
                  //moving, set face
                  if(fabs(vc->velocity.x) > fabs(vc->velocity.y))
                     spr->face = vc->velocity.x >= 0.0f ? st->get("run_right") : st->get("run_left");
                  else
                     spr->face = vc->velocity.y >= 0.0f ? st->get("run_down") : st->get("run_up");

                  //now reset anim speed base don velocity
                  auto &v = vc->velocity;
                  float mag = v.x * v.x + v.y * v.y;
           
                  spr->dtMultiplier = mag / (ac->maxVelocity * ac->maxVelocity);
            
               }
            }
         }
      }

      void attack()
      {
         stop();
         cc.setState(cc.buildAttackState());
      }
   };
   return StatePtr(new MoveState(*this));
}

StatePtr CharacterController::buildAttackState()
{
   class AttackState : public CharacterState
   {
      CharacterController &cc;
   public:
      AttackState(CharacterController &cc):cc(cc){}

      void onEnter()
      {
         auto st = IOC.resolve<StringTable>();

         if(auto e = cc.m_entity.lock())
         if(auto spr = e->getComponent<SpriteComponent>())
         {
            spr->face = st->get("attack_right");
            spr->elapsedTime = 0.0f;
            spr->dtMultiplier = 1.0f;
            cc.m_taskDone = false;
         }
      }

      void updateAnimation()
      {
         if(auto e = cc.m_entity.lock())
         if(auto spr = e->getComponent<SpriteComponent>())
         {
            if(spr->elapsedTime > spr->sprite->getFace(spr->face)->animation->getLength())
            {
               cc.m_taskDone = true;
               cc.revertState();
               
            }
         }
      }
   };
   return StatePtr(new AttackState(*this));
}

void CharacterController::setState(StatePtr state)
{
   if(!m_states.empty())
      m_states.top()->onExit();

   m_states.push(std::move(state));
   m_states.top()->onEnter();
}

void CharacterController::revertState()
{
   if(m_states.size() > 1)
   {
      m_states.top()->onExit();
      m_states.pop();
      m_states.top()->onEnter();
   }
}

bool CharacterController::currentTaskDone(){return m_taskDone;}

void CharacterController::move(Float2 vector)
{
   if(!m_states.empty())
      m_states.top()->move(vector);
}

void CharacterController::stop()
{
   if(!m_states.empty())
      m_states.top()->stop();
}

void CharacterController::updateAnimation()
{
   if(!m_states.empty())
      m_states.top()->updateAnimation();
}

void CharacterController::attack()
{
   if(!m_states.empty())
      m_states.top()->attack();
}