#pragma once

#include <string>
#include <unordered_map>

struct StringHash
{
   size_t operator()(const char *str) const
   {
      size_t out = 5381;
      while(*str)
         out = out << 5 + out << 1 + (*str++);

      return out;
   }
};

struct StringComp
{
   bool operator()(const char *lhs, const char *rhs) const
   {
      return strcmp(lhs, rhs) == 0;
   }
};

typedef std::string* InternString; 

class StringTable
{
   std::unordered_map<const char *, std::string, StringHash, StringComp> m_table;
public:
   static const int ID = 2615106637;

   std::string *get(const char *str)
   {
      auto iter = m_table.find(str);
      if(iter == m_table.end())
      {
         std::string temp(str);
         const char *tempStrPtrBadVariableName = (const char*)malloc(temp.size() + 1);
         strcpy((char*)tempStrPtrBadVariableName, temp.c_str());
         iter = m_table.insert(std::make_pair(tempStrPtrBadVariableName, std::move(temp))).first;

      } 
      
      return &iter->second;
   }

   std::string *get(const std::string &str)
   {
      return get(str.c_str());
   }

};