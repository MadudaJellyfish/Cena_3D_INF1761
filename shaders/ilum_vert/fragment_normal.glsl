#version 410

uniform sampler2D normalMap;
uniform sampler2D decal;


uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;


in data {
	vec3 dirLight;
	vec2 mapCoords;
	vec3 eye_v;
}v;


vec3 expand(vec3 v)
{
	return (v-0.5)*2;
}

out vec4 fcolor;

void main()
{
	vec3 light = normalize(v.dirLight);

	vec3 normalTex = texture(normalMap,v.mapCoords).rgb;
	vec3 normal = normalize(expand(normalTex));

	vec3 e = normalize(v.eye_v);

	float ndotl = dot(normal,light);

	vec4 ilumination = mamb*lamb + mdif * ldif * max(0,ndotl); 
    if (ndotl > 0) {
    vec3 refl = normalize(reflect(-light,normal));
    ilumination += mspe * lspe * pow(max(0,dot(refl,e)),mshi); 
    }

	vec4 textColor = texture(decal, v.mapCoords);

	fcolor = ilumination*textColor;
}