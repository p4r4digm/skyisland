#pragma once

#include "Vector.h"
#include "Entity.h"
#include <memory>
#include <stack>

class CharacterState
{
public:
   virtual void onEnter(){}
   virtual void onExit(){}

   virtual void move(Float2 vector){}
   virtual void stop(){}
   virtual void updateAnimation(){}
   virtual void attack(){}
};

typedef std::unique_ptr<CharacterState> StatePtr;

class CharacterController
{
   std::weak_ptr<Entity> m_entity;
   float m_accel, m_runAccel, m_maxVelocity, m_friction, m_minAnimSpeed, m_maxAnimSpeed;
   Float2 m_facing;

   std::stack<StatePtr> m_states;

   void setState(StatePtr state);
   void revertState();

   StatePtr buildMoveState();
   StatePtr buildAttackState();

   bool m_taskDone;
public:   

   CharacterController(std::weak_ptr<Entity> entity);
   void move(Float2 vector);
   void stop();
   void updateAnimation();
   void attack();

   bool currentTaskDone();

   friend CharacterState;
};