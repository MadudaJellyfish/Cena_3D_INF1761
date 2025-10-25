
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
#include "cube.h"
#include "sphere.h"
#include "cylinder.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "light.h"
#include "polyoffset.h"
#include "texture.h"

#include <iostream>
#include <cassert>

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
  //camera->SetOrtho(true);
  arcball = camera->CreateArcball();

  //LightPtr light = ObjLight::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  LightPtr light = Light::Make(0.0f,0.0f,0.0f,1.0f,"camera");
  light->SetPosition(5, 3, 0);
  light->SetDiffuse(1.0f, 1.0f, 1.0f);
  light->SetAmbient(0.5f, 0.5f, 0.5f);

  AppearancePtr white = Material::Make(1.0f, 1.0f, 1.0f);
  AppearancePtr light_greenyellow = Material::Make(0.62f,0.76f,0.19f);
  AppearancePtr ocean_green = Material::Make(0.0f,0.45f,0.45f);
  AppearancePtr beutifull_green = Material::Make(0.0f, 0.75f, 0.68f);
  AppearancePtr light_green = Material::Make(0.5f, 0.0f, 0.0f);

  AppearancePtr earth = Texture::Make("decal", "images/earth.jpg");
  AppearancePtr mercury = Texture::Make("decal", "images/mercury.jpg");
  
  //cilindro
  TransformPtr trf_cilindro = Transform::Make();
  trf_cilindro->Scale(0.3f, 1.0f, 0.3f);
  trf_cilindro->Translate(-2.0f, 0.0f, -2.0f);

  //mesa
  TransformPtr trf1 = Transform::Make();
  trf1->Scale(3.0f,0.3f,3.0f);
  trf1->Translate(0.0f,-1.0f,0.0f);

  //bloquinho
  TransformPtr trf_bloquinho = Transform::Make();
  trf_bloquinho->Scale(1.0f, 0.25f, 1.0f);
  trf_bloquinho->Translate(-0.5f, 0.0f, 0.5f);



  //esfera em cima do bloquinho
  TransformPtr trf_esfera_2 = Transform::Make();
  trf_esfera_2->Scale(0.2f, 0.2f, 0.2f);
  trf_esfera_2->Translate(-2.0f, 2.3f, 2.0f);


  //esfera em cima da mesa
  TransformPtr trf2 = Transform::Make();
  trf2->Scale(0.45f,0.45f,0.45f);
  trf2->Translate(1.5f,1.0f,1.5f);

  Error::Check("before shps");
  ShapePtr cube = Cube::Make();
  Error::Check("before sphere");
  ShapePtr sphere = Sphere::Make();
  Error::Check("before bloquinho");
  ShapePtr bloquinho = Cube::Make();
  Error::Check("before esfera bloquinho");
  ShapePtr esfera_bloquinho = Sphere::Make();
  Error::Check("before cilindro");
  ShapePtr cilindro = Cylinder::Make();
  Error::Check("after shps");


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
  NodePtr esfera2 = Node::Make(trf_esfera_2, { light_green }, { esfera_bloquinho });

  NodePtr root = Node::Make(shader,
    {Node::Make(trf1,{ocean_green},{cube}),
      Node::Make(trf_bloquinho, {beutifull_green}, {bloquinho}),
     Node::Make(trf_esfera_2,{light_green},{esfera_bloquinho}),
     Node::Make(shd_tex, trf2,{white, earth},{sphere}),
     Node::Make(shd_tex, trf_cilindro,{white, mercury},{cilindro})
    }
  );
  scene = Scene::Make(root);
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

  while(!glfwWindowShouldClose(win)) {
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

