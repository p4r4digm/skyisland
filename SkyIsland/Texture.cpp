#include "Texture.h"

#include "IL\ilut.h"
#include <GLFW/glfw3.h>

Texture::Texture(std::string filePath):
   m_filename(filePath), m_isLoaded(false), 
   m_bitsPerPixel(0), m_size(Int2()), m_glHandle(0)
{
}

Int2 Texture::getSize(){return m_size;}
int Texture::getBitsPerPixel(){return m_bitsPerPixel;}
unsigned int Texture::getGLHandle(){return m_glHandle;}
std::string Texture::getFilePath(){return m_filename;}
bool Texture::isLoaded(){return m_isLoaded;}

void Texture::acquire()
{
   m_glHandle = ilutGLLoadImage((char*)m_filename.c_str());
   m_size.x = ilGetInteger(IL_IMAGE_WIDTH);
   m_size.y = ilGetInteger(IL_IMAGE_WIDTH);
   m_bitsPerPixel = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

   m_isLoaded = true;
}

void Texture::release()
{
   glDeleteTextures(1, &m_glHandle);
   m_isLoaded = false;
}
