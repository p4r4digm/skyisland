#include "CharacterController.h"
#include "PhysicsComponents.h"
#include "GraphicComponents.h"
#include "PositionComponent.h"
#include "Animations.h"
#include "IOCContainer.h"
#include "Application.h"
#include "CharacterEntities.h"
#include "TextComponent.h"
#include "SkeletalNodeComponent.h"

CharacterController::CharacterController(std::weak_ptr<Entity> entity):
   m_entity(std::move(entity))
{
   m_taskDone = false;
   m_accel = 0.25f;
   m_runAccel = 0.5f;
   m_maxVelocity = 10.0f;
   m_friction = 0.25f;

   auto st = IOC.resolve<StringTable>();
   f_standDown = st->get("idledown");
   f_standUp = st->get("idleup");
   f_standRight = st->get("idleright");
   f_standLeft = st->get("idleleft");
   f_runUp = st->get("walkup");
   f_runDown = st->get("run_down");
   f_runLeft = st->get("run_left");
   f_runRight = st->get("run_right");
   f_attackLeft = st->get("attack_left");
   f_attackRight = st->get("attack_right");
   f_attackUp = st->get("attack_up");
   f_attackDown = st->get("attack_down");


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
            if(auto skel = e->getComponent<SkeletonComponent>())
            {
               if(vc->velocity == Float2())
               {
                  skel->dtMultiplier = 1.0f;
                  //stopped, set face

                  //left/right
                  if(fabs(cc.m_facing.x) > fabs(cc.m_facing.y))
                  {
                     //right
                     if(cc.m_facing.x >= 0.0f )
                     {
                        if(skel->playingAnimation != st->get("walkright") && skel->playingAnimation != st->get("idleright"))
                           skel->changeAnim(st->get("faceright"));

                        skel->changeAnim(st->get("idleright"));
                     }
                     //left
                     else
                     {
                        if(skel->playingAnimation != st->get("walkleft") && skel->playingAnimation != st->get("idleleft"))
                           skel->changeAnim(st->get("faceleft"));

                        skel->changeAnim(st->get("idleleft"));
                     }
                  }
                  //up/down   
                  else
                  {
                     //down
                     if(cc.m_facing.y >= 0.0f)
                     {
                        if(skel->playingAnimation != st->get("walkup") && skel->playingAnimation != st->get("idleup"))
                           skel->changeAnim(st->get("faceup"));

                        skel->changeAnim(st->get("idleup"));
                     }
                     //up
                     else
                     {
                        if(skel->playingAnimation != st->get("walkdown") && skel->playingAnimation != st->get("idledown"))
                           skel->changeAnim(st->get("facedown"));

                        skel->changeAnim(st->get("idledown"));
                     }
                  }
                     
               }
               else
               {
                  //moving, set face
                  if(fabs(vc->velocity.x) > fabs(vc->velocity.y))
                  {
                     if(vc->velocity.x >= 0.0f)
                     {
                        if(skel->playingAnimation != st->get("walkright") && skel->playingAnimation != st->get("idleright"))
                           skel->changeAnim(st->get("faceright"));

                        skel->changeAnim(st->get("walkright"));
                     }
                     else
                     {
                        if(skel->playingAnimation != st->get("walkleft") && skel->playingAnimation != st->get("idleleft"))
                           skel->changeAnim(st->get("faceleft"));

                        skel->changeAnim(st->get("walkleft"));
                     }
                  }
                     
                  else                     
                     if( vc->velocity.y >= 0.0f)
                     {
                        if(skel->playingAnimation != st->get("walkdown") && skel->playingAnimation != st->get("idledown"))
                           skel->changeAnim(st->get("facedown"));

                        skel->changeAnim(st->get("walkdown"));
                     }
                     else
                     {
                        if(skel->playingAnimation != st->get("walkup") && skel->playingAnimation != st->get("idleup"))
                           skel->changeAnim(st->get("faceup"));

                        skel->changeAnim(st->get("walkup"));
                     }



                  //now reset anim speed base don velocity
                  auto &v = vc->velocity;
                  float mag = v.x * v.x + v.y * v.y;

                  skel->dtMultiplier = mag / (ac->maxVelocity * ac->maxVelocity) * 2.0f;

               }
            }
         }
      }

      void attack()
      {
         stop();
         cc.setState(cc.buildAttackState());
      }

      void damage(const AttackComponent &ac)
      {
         stop();
         cc.setState(cc.buildDamagedState(ac));
      }
   };
   return StatePtr(new MoveState(*this));
}

StatePtr CharacterController::buildAttackState()
{
   class AttackState : public CharacterState
   {
      CharacterController &cc;
      std::shared_ptr<Entity> slashEntity;

      void checkForEnd(SpriteComponent &spr)
      {
         if(spr.elapsedTime > spr.sprite->getFace(spr.face)->animation->getLength())
         {
            cc.m_taskDone = true;
            cc.revertState();
         }
      }

   public:
      AttackState(CharacterController &cc):cc(cc){}

      ~AttackState()
      {
         slashEntity->markedForDeletion = true;
      }

      void onEnter()
      {
         if(auto e = cc.m_entity.lock())
         if(auto spr = e->getComponent<SpriteComponent>())
         if(auto pos = e->getComponent<PositionComponent>())
         if(auto gb = e->getComponent<GraphicalBoundsComponent>())
         {


            ///create sword swipe
            slashEntity = CharacterEntities::buildSwordAttack(cc.m_entity);
            slashEntity->addComponent<PositionBindComponent>(std::make_shared<PositionBindComponent>(cc.m_entity, Float2(0.0f, 0.1f)));
            slashEntity->getComponent<PositionComponent>()->pos = pos->pos;
            slashEntity->getComponent<GraphicalBoundsComponent>()->size = gb->size;
            slashEntity->getComponent<AttackComponent>()->attackDirection = Float2(cc.m_facing.x, -cc.m_facing.y);
            slashEntity->addToScene(cc.m_entity.lock()->getScene());
            //slashEntity->getComponent<SpriteComponent>()->dtMultiplier = 0.5f;

            spr->face = cc.f_attackRight;
            spr->elapsedTime = 0.0f;
            spr->dtMultiplier = 1.0f;
            cc.m_taskDone = false;
         }
      }

      void updateOnScreen()
      {
         if(auto e = cc.m_entity.lock())
         if(auto spr = e->getComponent<SpriteComponent>())
         {
            checkForEnd(*spr);
         }
      }

      void updateOffScreen()
      {
         if(auto e = cc.m_entity.lock())
         if(auto spr = e->getComponent<SpriteComponent>())
         {
            spr->updateTime();
            checkForEnd(*spr);
         }
      }

      void damage(const AttackComponent &ac)
      {
         slashEntity->markedForDeletion = true;
         stop();
         cc.m_taskDone = true;
         cc.replaceState(cc.buildDamagedState(ac));
      }
   };
   return StatePtr(new AttackState(*this));
}

StatePtr CharacterController::buildDamagedState(const AttackComponent &ac)
{
   class DamagedState : public CharacterState
   {
      CharacterController &cc;
      const AttackComponent &ac;
      float startTime;
   public:
      DamagedState(CharacterController &cc, const AttackComponent &ac):
         cc(cc), ac(ac){}

      void onEnter()
      {
         if(auto e = cc.m_entity.lock())
         if(auto fc = e->getComponent<FrictionComponent>())
         if(auto vc = e->getComponent<VelocityComponent>())
         if(auto ec = e->getComponent<ElevationComponent>())
         if(auto spr = e->getComponent<SpriteComponent>())
         if(auto app = IOC.resolve<Application>())
         {
            ec->impulse = 10.0f;
            startTime = app->getTime();
            vc->velocity = (Float2)ac.attackDirection * 10.0f;
            fc->friction = cc.m_friction;
            //spr->dtMultiplier = 0.0f;
            spr->elapsedTime = 0.0f;

            auto dm = CharacterEntities::buildDamageMarker(e);
            if(auto tc = dm->getComponent<TextComponent>())
            {
               tc->str = "9999";
               if(auto ec2 = dm->getComponent<ElevationComponent>())
               {
                  ec2->elevation = 50.0f;
                  ec2->impulse = 15.0f;
               }

            }

            dm->addToScene(e->getScene());

         }
      }

      void updateOnScreen()
      {
         if(auto app = IOC.resolve<Application>())
         {
            if(app->getTime() - startTime > 0.50f)
            {
               cc.revertState();
            }
         }

      }

      void updateOffScreen()
      {
         updateOnScreen();
      }

   };
   return StatePtr(new DamagedState(*this, ac));
}

void CharacterController::setState(StatePtr state)
{
   if(!m_states.empty())
      m_states.top()->onExit();

   m_states.push(std::move(state));
   m_states.top()->onEnter();
}

void CharacterController::replaceState(StatePtr state)
{
   if(!m_states.empty())
      m_states.top()->onExit();

   m_states.pop();
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

void CharacterController::updateOnScreen()
{
   if(!m_states.empty())
      m_states.top()->updateOnScreen();
}

void CharacterController::updateOffScreen()
{
   if(!m_states.empty())
      m_states.top()->updateOffScreen();
}

void CharacterController::damage(const AttackComponent &ac)
{
   if(!m_states.empty())
      m_states.top()->damage(ac);
}
