#include "quad.h"
#include "error.h"
#include "grid.h"
#include <vector>

#include <iostream>

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
#endif

QuadPtr Quad::Make (std::vector<glm::vec2> coord, std::vector<glm::vec2> coordText)
{
  return QuadPtr(new Quad(coord, coordText));
}

Quad::Quad (std::vector<glm::vec2> coord, std::vector<glm::vec2> coordText)
{  
  // create VAO
  glGenVertexArrays(1,&m_vao);
  glBindVertexArray(m_vao);
  // create coord buffer
  GLuint coord_buffer;
  GLuint coord_text_buffer;

  glGenBuffers(1, &coord_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, coord_buffer);
  glBufferData(GL_ARRAY_BUFFER, coord.size()  * 8 , coord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &coord_text_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, coord_text_buffer);
  glBufferData(GL_ARRAY_BUFFER, coordText.size()  * 8 , coordText.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

Quad::~Quad () 
{
}

void Quad::Draw (StatePtr )
{
  glBindVertexArray(m_vao);

  //glVertexAttrib3f(1,0.0f,0.0f,1.0f); // constant for all vertices
  //glVertexAttrib3f(2,1.0f,0.0f,0.0f); // constant for all vertices
  glDrawArrays(GL_TRIANGLE_FAN,0,4);
  glBindVertexArray(0);
}