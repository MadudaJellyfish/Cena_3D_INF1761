
#ifdef _WIN32
#include <windows.h>
// Use glad to initialize OpenGL context on Windows
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#elif __APPLE__
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#elif __linux__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

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
#include "texcube.h"
#include "skyBlock.h"

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
	float getAcummulateAngle() const { return m_angle; }

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
static MoveAstroPtr move_terra;
static MoveAstroPtr move_lua;
static bool canGoToTerra = false;
static bool canGoToLua = false;

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE); 
  glCullFace(GL_BACK);
  //glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);

  /// fim do n� de fundo
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

  AppearancePtr sky = TexCube::Make("sky","images/cubemap.png");
  AppearancePtr sun = Texture::Make("decal", "images/sun.jpg");
  AppearancePtr earth = Texture::Make("decal", "images/earth.jpg");
  AppearancePtr earth_normal = Texture::Make("normalMap", "images/earth-normal.png");
  AppearancePtr mercury = Texture::Make("decal", "images/mercury.jpg");
  AppearancePtr mercury_normal = Texture::Make("normalMap", "images/mercury-normal.jpg");
  AppearancePtr moon = Texture::Make("decal", "images/moon.jpeg");
  AppearancePtr moon_normal = Texture::Make("normalMap", "images/moon-normal.png");
  

  //transf. sol
  TransformPtr trf_sol = Transform::Make();
  trf_sol->Scale(0.5f, 0.5f, 0.5f);

  //transf. merc�rio
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

  //contru��o das geometrias
  Error::Check("before shps");
  ShapePtr ceu = SkyBlock::Make();
  Error::Check("before sun");
  ShapePtr sol = Sphere::Make();
  Error::Check("before terra");
  ShapePtr terra = Sphere::Make();
  Error::Check("before lua");
  ShapePtr lua = Sphere::Make();
  Error::Check("before merc�rio");
  ShapePtr mercurio = Sphere::Make();
  Error::Check("after shps");
  //

  //shader rugosidade
  ShaderPtr shd_rugos = Shader::Make(light, "world");
  shd_rugos->AttachVertexShader("shaders/ilum_vert/vertex_normal.glsl");
  shd_rugos->AttachFragmentShader("shaders/ilum_vert/fragment_normal.glsl");
  shd_rugos->Link();


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

  ShaderPtr shd_sky = Shader::Make();
  shd_sky->AttachVertexShader("shaders/ilum_vert/vertex_sky.glsl");
  shd_sky->AttachFragmentShader("shaders/ilum_vert/fragment_sky.glsl");
  shd_sky->Link();
 
  // build scene

  ///criacao do n�s da cena

  auto lua_node = Node::Make(shd_rugos,trf_lua, { white, moon, moon_normal }, { lua });
  auto lua_orbita_node = Node::Make(trf_orbita_lua, {}, {}, { lua_node });

  auto terra_node = Node::Make(shd_rugos,trf_terra, { white, earth, earth_normal }, { terra }, {});
  auto terra_rotacao_node = Node::Make(trf_rotacao_terra, {}, {}, { terra_node});
  auto terra_orbita_node = Node::Make(trf_orbita_terra, {}, {}, { terra_rotacao_node, lua_orbita_node });

  auto mercurio_node = Node::Make(shd_rugos,trf_mercurio, { white, mercury, mercury_normal }, { mercurio }, {});
  auto mercurio_orbita_node = Node::Make(trf_orbita_mercurio, {}, {}, { mercurio_node });

  auto sol_node = Node::Make(shd_tex,trf_sol, { sun_material, sun }, { sol }, { mercurio_orbita_node, terra_orbita_node });

  ///

  NodePtr root = Node::Make(shader, {white, sky}, {
    Node::Make(shd_sky, {sky}, {ceu}),
    {sol_node}
});
  
  move_terra = MoveAstro::Make(trf_orbita_terra, 1.0f, 3.0f);
  move_lua = MoveAstro::Make(trf_orbita_lua, 2.0f, 1.5f);
  scene = Scene::Make(root);
  scene->AddEngine(move_terra); // Raio maior para a Terra
  scene->AddEngine(move_lua); // Raio menor para a Lua (relativo � Terra)
  scene->AddEngine(MoveAstro::Make(trf_orbita_mercurio, 1.5f, 1.5f)); // Raio menor para Merc�rio
  scene->AddEngine(AxisRotation::Make(trf_rotacao_terra, 20.f, 0.45f)); // Velocidade de rota��o menor
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

  if (key == GLFW_KEY_T && action == GLFW_PRESS) 
  {
      if(canGoToTerra)
      {
          glCullFace(GL_BACK);
          canGoToTerra = false;
		  
      } 
	  else
      {
          glCullFace(GL_FRONT);
          canGoToTerra = true;
		  
      }
        
  }
  if(key == GLFW_KEY_L && action == GLFW_PRESS)
  {
      if (canGoToLua)
      {
          canGoToLua = false;
          
      }
      else
      {
          canGoToLua = true;
		  
      }
  }
}

static void changeCameraPos()
{
	float raioTerra = 1.35f;
    float angleTerra = move_terra->getAcummulateAngle();
    if (canGoToTerra)
    {
        camera->SetEye(raioTerra*cos(angleTerra), 0.0f, raioTerra*sin(angleTerra));
        camera->SetCenter(0.0f, 0.0f, 0.0f);
    }
    else if (canGoToLua)
    {
        float angle = move_lua->getAcummulateAngle();
		float raioLua = 1.5f; // dist�ncia da Terra + dist�ncia da Lua em rela��o � Terra

        glm::vec3 terra_pos(raioTerra * cos(angleTerra),
            0.0f,
            raioTerra * sin(angleTerra));

        glm::vec3 lua_pos_rel(raioLua * cos(angle),
            0.0f,
            raioLua * sin(angle));

        glm::vec3 moon_world_pos = terra_pos + lua_pos_rel;

        camera->SetCenter(moon_world_pos.x, moon_world_pos.y, moon_world_pos.z);

        
        camera->SetEye(moon_world_pos.x - 2.0f, 
            moon_world_pos.y + 1.0f, 
            moon_world_pos.z + 2.0f);

    }
    else
    {
        camera->SetEye(viewer_pos[0], viewer_pos[1], viewer_pos[2]);
        camera->SetCenter(0.0f, 0.0f, 0.0f);
    }
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
	changeCameraPos();
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
#if defined(__linux__) && defined(__glad_h_)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

