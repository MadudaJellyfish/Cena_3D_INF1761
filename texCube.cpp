#include "texCube.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifdef _WIN32
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
#endif

TexCubePtr TexCube::Make (const std::string& varname, const std::string& filename)
{
  return TexCubePtr(new TexCube(varname,filename));
}

static unsigned char* extract_subimage(int W, int n, const unsigned char* data,
                                        int x, int y, int w, int h)

{
    unsigned char* img = (unsigned char*) malloc(w*h*n);
    for (int i=0; i<h; ++i) {
        memcpy(img+(i*w)*n,data+((y+i)*W+x)*n,w*n);
    }
    return img;
}


TexCube::TexCube (const std::string& varname, const std::string& filename)
: m_varname(varname)
{
  ImagePtr img = Image::Make(filename);
  glGenTextures(1,&m_tex);
  glBindTexture(GL_TEXTURE_CUBE_MAP,m_tex);

  // subimages’ dimension
  int w = img->GetWidth()/4, h = img->GetHeight()/3;
  int x[] = {2*w,  0,  w,  w,  w,3*w};
  int y[] = {  h,  h,  0,2*h,  h,  h};
  GLenum face[] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X,   // right
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   // left
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,   // top
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,   // bottom
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   // front
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,   // back
  };
  for (int i=0; i<6; ++i) {
      unsigned char* imgFace = extract_subimage(img->GetWidth(),img->GetNChannels(), img->GetData(), x[i],y[i],w,h);
      glTexImage2D(face[i],0,GL_RGB,w,h,0,
                  img->GetNChannels()==3?GL_RGB:GL_RGBA,GL_UNSIGNED_BYTE,imgFace);
      free(imgFace);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP ,GL_TEXTURE_MIN_FILTER ,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP ,GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP ,GL_TEXTURE_WRAP_S ,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP ,GL_TEXTURE_WRAP_T ,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP ,GL_TEXTURE_WRAP_R ,GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}

TexCube::~TexCube ()
{
}

unsigned int TexCube::GetTexId () const
{
  return m_tex;
}

void TexCube::Load (StatePtr st)
{
  ShaderPtr shd = st->GetShader();
  shd->ActiveTexture(m_varname.c_str());
  glBindTexture(GL_TEXTURE_CUBE_MAP,m_tex);
}

void TexCube::Unload (StatePtr st)
{
  ShaderPtr shd = st->GetShader();
  shd->DeactiveTexture();
}



