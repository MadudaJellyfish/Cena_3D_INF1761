#include <memory>
class Quad;
using QuadPtr = std::shared_ptr<Quad>; 

#ifndef QUAD_H
#define QUAD_H

#include "shape.h"
#include "state.h"

class Quad : public Shape {
  unsigned int m_vao;
protected:
  Quad (std::vector<glm::vec2> coord, std::vector<glm::vec2> coordText);
public:
  static QuadPtr Make (std::vector<glm::vec2> coord, std::vector<glm::vec2> coordText);
  virtual ~Quad ();
  virtual void Draw (StatePtr st);
};
#endif