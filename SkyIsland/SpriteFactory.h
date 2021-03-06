#pragma once

#include "Sprite.h"
#include "IAnimationStrategy.h"
#include "StringTable.h"
#include <map>

struct FSNode
{
   FSNode(){}
   FSNode(InternString path):path(path){}

   InternString path;
   std::map<InternString, FSNode> children;

   FSNode &operator[](InternString dir){return children[dir];}
};

class SpriteFactory
{
   FSNode m_assetIndex;

public:
   static const unsigned int ID = 2909225244;
   std::shared_ptr<Sprite> buildSprite(InternString filepath, IAnimationStrategy *animStrat);
   void buildAssetIndex(std::string assetRoot);
};
