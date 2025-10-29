#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent; 
layout(location = 3) in vec2 texcoord;


uniform mat4 Mv; 
uniform mat4 Mvp;
uniform mat4 Mn; 
uniform vec4 lpos;


out data {
	vec3 dirLight;
	vec2 mapCoords;
	vec3 eye_v;
}v;


void main(void)
{
	gl_Position = Mvp * coord;
	v.mapCoords = texcoord;

	vec3 veye = vec3(Mv*coord);
  	
	vec3 T_eye = normalize(vec3(Mn * vec4(tangent, 0.0)));
    vec3 N_eye = normalize(vec3(Mn * vec4(normal, 0.0)));
    vec3 B_eye = cross(N_eye, T_eye); 
    mat3 TBN = transpose(mat3(T_eye, B_eye, N_eye));

    vec3 lightDir_eye = vec3(lpos) - veye;
    v.dirLight = TBN * lightDir_eye;

	vec3 eyeDir_eye = -veye;
    v.eye_v = TBN * eyeDir_eye;

}