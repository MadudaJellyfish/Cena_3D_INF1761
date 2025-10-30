#include <memory>

class TexCube;
using TexCubePtr = std::shared_ptr<TexCube>;

#ifndef TEXTCUBE_H
#define TEXTCUBE_H

#include "appearance.h"
#include <glm/glm.hpp>
#include <string>

class TexCube : public Appearance {
  unsigned int m_tex;
  std::string m_varname;
protected:
  TexCube (const std::string& varname, const std::string& filename);
  TexCube (const std::string& varname, int width, int height);
  TexCube (const std::string& varname, const glm::vec3& texel);
public:
  static TexCubePtr Make (const std::string& varname, const std::string& filename);
  virtual ~TexCube ();
  unsigned int GetTexId () const;
  virtual void Load (StatePtr st);
  virtual void Unload (StatePtr st);
};

#endif
