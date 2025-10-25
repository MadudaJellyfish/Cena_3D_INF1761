#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

//iluminacao por fragmento
out vec3 normal_v;
out vec3 light_v;
out vec3 eye_v;

void main (void) 
{
  vec3 veye = vec3(Mv*coord);
  eye_v = -veye;
  normal_v = normalize(vec3(Mn*vec4(normal, 0.0f)));

  vec3 light;
  if (lpos.w == 0) 
    light = normalize(vec3(lpos));
  else 
    light = normalize(vec3(lpos)-veye); 
  light_v = light;

  gl_Position = Mvp*coord; 
}

