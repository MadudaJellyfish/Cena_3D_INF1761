#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent; 
layout(location = 3) in vec2 texcoord;


uniform mat4 Mv; 
uniform mat4 Mvp;
uniform mat4 lpos;


out data {
	vec3 dirLight;
	vec2 mapCoords;
	vec3 eye_v
}v;


void main(void)
{
	gl_Position = Mvp * coord;
	v.mapCoords = texcoord;

	vec3 veye = vec3(Mv*coord);
  	eye_v = -veye;

	vec3 binormal = cross(normal, tangent);
	mat3 TBN = transpose(mat3(tangent, binormal, normal));

	eye_v = TBN * (eye_v - vec3(coord)); //no espaco tangente
	v.dirLight = TBN * (lpos - coord);
}