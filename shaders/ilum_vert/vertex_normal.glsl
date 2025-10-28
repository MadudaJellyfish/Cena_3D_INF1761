#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent; 
layout(location = 3) in vec2 texcoord;

uniform mat4 Mvp;

uniform mat4 lpos;

out vec3 dirLight;
out vec2 mapCoords;

void main(void)
{
	gl_Position = Mvp * coord;
	mapCoords = texcoord;

	vec3 binormal = cross(normal, tangent);
	mat3 rotation = transpose(mat3(tangent, binormal, normal));

	dirLight = rotation * (lpos - gl_Position);
}