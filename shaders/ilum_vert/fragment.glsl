#version 410

in vec3 normal_v;
in vec3 light_v;
in vec3 eye_v;

uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;

out vec4 fcolor;

out vec4 scolor;

uniform samplerCube sky;

const float ka = 0.4f;
const vec4 ma = vec4(1.0f,1.0f,1.0f,1.0f);
const vec4 md = vec4(1.0f,1.0f,1.0f,1.0f);
const vec4 ms = vec4(1.0f,1.0f,1.0f,1.0f);
const float shi = 64.0f;


void main (void)
{
  vec3 n = normalize(normal_v);
  vec3 l = normalize(light_v);
  vec3 e = normalize(eye_v);

  float ndotl = dot(n,l);
  vec4 color = mamb*lamb + mdif * ldif * max(0,ndotl); 
  if (ndotl > 0) {
  vec3 refl = normalize(reflect(-l,n));
  color += mspe * lspe * pow(max(0,dot(refl,e)),mshi); 
  }

  fcolor = color;
  
}

