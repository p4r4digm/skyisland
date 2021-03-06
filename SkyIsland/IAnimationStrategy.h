#pragma once

#include "IAnimation.h"
#include "StringTable.h"
#include <memory>
#include <string>

class IAnimationStrategy
{
public:
   virtual ~IAnimationStrategy(){}

   virtual std::unique_ptr<IAnimation<int>> createAnimation(int frameCount)=0;
};
