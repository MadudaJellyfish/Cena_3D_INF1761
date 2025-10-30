#include <memory>
class SkyBlock;
using SkyBlockPtr = std::shared_ptr<SkyBlock>;

#ifndef SKYBLOCK_H
#define SKYBLOCK_H

#include "shape.h"
#include "state.h"

class SkyBlock : public Shape {
  unsigned int m_vao;
  unsigned int m_coord_buffer;
protected:
  SkyBlock ();
public:
  static SkyBlockPtr Make ();
  virtual ~SkyBlock ();
  virtual void Draw (StatePtr st);
};
#endif