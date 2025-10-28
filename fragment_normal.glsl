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
	vec3 eye_v
}v;


vec3 expand(vec3 v)
{
	return (v-0.5)*2;
}

out vec4 fcolor;

void main()
{
	vec3 light = normalize(dirLight);

	vec3 normalTex = texture(normalMap,mapCoord);
	vec3 normal = normalize(expand(normalTex));

	vec3 e = normalize(eye_v);

	float ndotl = dot(n,light);

	vec4 ilumination = mamb*lamb + mdif * ldif * max(0,ndotl); 
    if (ndotl > 0) {
    vec3 refl = normalize(reflect(-l,n));
    ilumination += mspe * lspe * pow(max(0,dot(refl,e)),mshi); 
    }
	
	vec4 textColor = texture(decal, v.mapCoord);

	fcolor = ilumination*textColor;
}