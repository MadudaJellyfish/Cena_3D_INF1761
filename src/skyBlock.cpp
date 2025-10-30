#include "skyBlock.h"
#include "camera.h"

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

SkyBlockPtr SkyBlock::Make ()
{
  return SkyBlockPtr(new SkyBlock());
}

SkyBlock::SkyBlock ()
{  
  float coords[] = { 
    // back  (z = -1)
    -1.f,  1.f, -1.f,  -1.f, -1.f, -1.f,   1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,   1.f,  1.f, -1.f,  -1.f,  1.f, -1.f,

    // front (z = +1)
    -1.f,  1.f,  1.f,  -1.f, -1.f,  1.f,   1.f, -1.f,  1.f,
     1.f, -1.f,  1.f,   1.f,  1.f,  1.f,  -1.f,  1.f,  1.f,

    // left  (x = -1)
    -1.f,  1.f,  1.f,  -1.f,  1.f, -1.f,  -1.f, -1.f, -1.f,
    -1.f, -1.f, -1.f,  -1.f, -1.f,  1.f,  -1.f,  1.f,  1.f,

    // right (x = +1)
     1.f,  1.f, -1.f,   1.f,  1.f,  1.f,   1.f, -1.f,  1.f,
     1.f, -1.f,  1.f,   1.f, -1.f, -1.f,   1.f,  1.f, -1.f,

    // top   (y = +1)
    -1.f,  1.f,  1.f,   1.f,  1.f,  1.f,   1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,  -1.f,  1.f, -1.f,  -1.f,  1.f,  1.f,

    // bottom(y = -1)
    -1.f, -1.f, -1.f,   1.f, -1.f, -1.f,   1.f, -1.f,  1.f,
     1.f, -1.f,  1.f,  -1.f, -1.f,  1.f,  -1.f, -1.f, -1.f
  };

  // create VAO
  glGenVertexArrays(1,&m_vao);
  glBindVertexArray(m_vao);
  // create coord buffer=
  glGenBuffers(1,&m_coord_buffer);
  glBindBuffer(GL_ARRAY_BUFFER,m_coord_buffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(coords),coords,GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);
}

SkyBlock::~SkyBlock () 
{
}

void SkyBlock::Draw (StatePtr st)
{
    // draw at camera position
    CameraPtr camera = st->GetCamera();
    glm::vec4 origin = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    glm::vec3 peye = glm::vec3(glm::inverse(camera->GetViewMatrix()) * origin);
    glm::mat4 m = glm::translate(glm::mat4(1.0f), peye);
    st->PushMatrix();
    st->MultMatrix(m);
    st->LoadMatrices();  // update loaded matrices
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_coord_buffer);
    glEnableVertexAttribArray(LOC::COORD);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindBuffer(GL_ARRAY_BUFFER, m_coord_buffer);
    glDisableVertexAttribArray(LOC::COORD);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    st->PopMatrix();
}

