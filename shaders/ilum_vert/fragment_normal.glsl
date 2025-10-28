#version 410

uniform sampler2D normalMap;

void main()
{
	normal = texture(normalMap, fs_in.TextCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
}