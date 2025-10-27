
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "transform.h"
#include "sphere.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "light.h"
#include "texture.h"
#include "color.h"
#include "quad.h"
#include "engine.h"

#include <iostream>
#include <cassert>


class MoveAstro;
using MoveAstroPtr = std::shared_ptr<MoveAstro>;
class MoveAstro : public Engine
{
    TransformPtr m_trf;
    float m_angular_velocity;
    float m_radius;
    float m_angle;

protected:
    MoveAstro(TransformPtr trf, float velocity, float radius)
        : m_trf(trf),
        m_angular_velocity(velocity),
        m_radius(radius),
        m_angle(0.0f)
    {
    }
public:
    static MoveAstroPtr Make(TransformPtr trf, float velocity, float radius)
    {
        return MoveAstroPtr(new MoveAstro(trf, velocity, radius));
    }
    virtual void Update(float dt)
    {
        m_angle += -dt * m_angular_velocity;

        m_trf->LoadIdentity();
       // m_trf->Rotate(0, m_angle, 0, 1);
        m_trf->Translate(m_radius*cos(m_angle), 0.0f, m_radius * sin(m_angle));
    }

};

class AxisRotation;
using AxisRotationPtr = std::shared_ptr<AxisRotation>;
class AxisRotation : public Engine
{
    TransformPtr m_trf;
    float m_angular_velocity;
    float m_scale;
    float m_angle;

protected:
    AxisRotation(TransformPtr trf, float velocity, float scale)
        : m_trf(trf),
        m_angular_velocity(velocity),
        m_scale(scale),
        m_angle(0.0f)
    {
    }
public:
    static AxisRotationPtr Make(TransformPtr trf, float velocity, float scale)
    {
        return AxisRotationPtr(new AxisRotation(trf, velocity, scale));
    }
    virtual void Update(float dt)
    {
        m_angle += -dt * m_angular_velocity;

        m_trf->LoadIdentity();
        m_trf->Scale(m_scale, m_scale, m_scale);
        m_trf->Rotate(m_angle,0, 1, 0);
    }

};


static float viewer_pos[3] = {2.0f, 3.5f, 4.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static ArcballPtr arcball;

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);

  /// fim do nó de fundo
  arcball = camera->CreateArcball();

  //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  LightPtr light = Light::Make(0.0f,0.0f,0.0f,1.0f,"world");
  light->SetPosition(0.0f, 0.0f, 0.0f);
  light->SetDiffuse(1.0f, 1.0f, 1.0f);
  light->SetAmbient(0.3f, 0.3f, 0.3f);
  light->SetSpecular(0.0f, 0.0f, 0.0f);

  AppearancePtr sun_material = Material::Make(1.0f, 0.0f, 0.0f);
  AppearancePtr white = Material::Make(1.0f, 1.0f, 1.0f);
  AppearancePtr light_greenyellow = Material::Make(0.62f,0.76f,0.19f);
  AppearancePtr ocean_green = Material::Make(0.0f,0.45f,0.45f);
  AppearancePtr beutifull_green = Material::Make(0.0f, 0.75f, 0.68f);
  AppearancePtr light_green = Material::Make(0.5f, 0.0f, 0.0f);

  AppearancePtr sun = Texture::Make("decal", "images/sun.jpg");
  AppearancePtr earth = Texture::Make("decal", "images/earth.jpg");
  AppearancePtr mercury = Texture::Make("decal", "images/mercury.jpg");
  AppearancePtr moon = Texture::Make("decal", "images/moon.jpeg");
  

  //transf. sol
  TransformPtr trf_sol = Transform::Make();
  trf_sol->Scale(0.5f, 0.5f, 0.5f);

  //transf. mercúrio
  TransformPtr trf_orbita_mercurio = Transform::Make();
  TransformPtr trf_mercurio = Transform::Make();
  trf_mercurio->Scale(0.15f, 0.15f, 0.15f);
  //trf_mercurio->Translate(10.0f, 1.0f, 1.5f);


  //transf. lua
  TransformPtr trf_lua = Transform::Make();
  trf_lua->Scale(0.2f, 0.2f, 0.2f);
  //trf_lua->Translate(18.0f, 1.0f, 1.5f);

  TransformPtr trf_orbita_lua = Transform::Make();


  //transf. terra
  TransformPtr trf_terra = Transform::Make();
  //trf_terra->Scale(0.45f,0.45f,0.45f);
  //trf_terra->Translate(10.0f,1.0f,1.5f);

  TransformPtr trf_orbita_terra = Transform::Make();
  TransformPtr trf_rotacao_terra = Transform::Make();

  //contrução das geometrias
  Error::Check("before shps");
  Error::Check("before sphere");
  ShapePtr sol = Sphere::Make();
  Error::Check("before terra");
  ShapePtr terra = Sphere::Make();
  Error::Check("before lua");
  ShapePtr lua = Sphere::Make();
  Error::Check("before mercúrio");
  ShapePtr mercurio = Sphere::Make();
  Error::Check("after shps");
  //

    // create shader
  ShaderPtr shader = Shader::Make(light,"world");
  shader->AttachVertexShader("shaders/ilum_vert/vertex.glsl");
  shader->AttachFragmentShader("shaders/ilum_vert/fragment.glsl");
  shader->Link();

  // Define a different shader for texture mapping
  // An alternative would be to use only this shader with a "white" texture for untextured objects
  ShaderPtr shd_tex = Shader::Make(light,"world");
  shd_tex->AttachVertexShader("shaders/ilum_vert/vertex_texture.glsl");
  shd_tex->AttachFragmentShader("shaders/ilum_vert/fragment_texture.glsl");
  shd_tex->Link();

  // build scene

  ///criacao do nós da cena

  auto lua_node = Node::Make(shd_tex,trf_lua, { white, moon }, { lua });
  auto lua_orbita_node = Node::Make(trf_orbita_lua, {}, {}, { lua_node });

  auto terra_node = Node::Make(shd_tex,trf_terra, { white, earth }, { terra }, {});
  auto terra_rotacao_node = Node::Make(trf_rotacao_terra, {}, {}, { terra_node, lua_orbita_node });
  auto terra_orbita_node = Node::Make(trf_orbita_terra, {}, {}, { terra_rotacao_node });

  auto mercurio_node = Node::Make(shd_tex,trf_mercurio, { white, mercury }, { mercurio }, {});
  auto mercurio_orbita_node = Node::Make(trf_orbita_mercurio, {}, {}, { mercurio_node });

  auto sol_node = Node::Make(shd_tex,trf_sol, { sun_material, sun }, { sol }, { mercurio_orbita_node, terra_orbita_node });

  ///



  NodePtr root = Node::Make(shader,
    {sol_node}
  );
  scene = Scene::Make(root);
  scene->AddEngine(MoveAstro::Make(trf_orbita_terra, 1.0f, 3.0f)); // Raio maior para a Terra
  scene->AddEngine(MoveAstro::Make(trf_orbita_lua, 3.0f, 1.5f)); // Raio menor para a Lua (relativo à Terra)
  scene->AddEngine(MoveAstro::Make(trf_orbita_mercurio, 1.5f, 1.5f)); // Raio menor para Mercúrio
  scene->AddEngine(AxisRotation::Make(trf_rotacao_terra, 20.f, 0.45f)); // Velocidade de rotação menor
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(int(x),int(y));
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(int(x),int(y));
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}
static void update(float dt)
{
    scene->Update(dt);
}

int main ()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);       // required for mac os
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GLFW_TRUE);  // option for mac os
#endif

  glfwSetErrorCallback(error);

  GLFWwindow* win = glfwCreateWindow(600,400,"Window title",nullptr,nullptr);
  assert(win);
  glfwSetFramebufferSizeCallback(win, resize);  // resize callback
  glfwSetKeyCallback(win, keyboard);            // keyboard callback
  glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback
  
  glfwMakeContextCurrent(win);
#ifdef _WIN32
  if (!gladLoadGL()) {
      printf("Failed to initialize GLAD OpenGL context\n");
      exit(1);
  }
#endif
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t - t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

