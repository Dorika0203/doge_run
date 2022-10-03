#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// inputs from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// output of the fragment shader
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform mat4	view_matrix;

uniform int		color_flag;

uniform float	shiness;
uniform vec4	light_position, Ia, Id, Is;
uniform vec4	solid_color;

uniform sampler2D TEX;

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 iK, float visibility)
{
	vec4 Ira = iK*Ia;														// ambient reflection
	vec4 Ird = max(iK*dot(l,n)*Id,0.0) * visibility;						// diffuse reflection
	vec4 Irs = max(iK*pow(dot(h,n),shiness)*Is,0.0) * visibility;			// specular reflection
	return Ira + Ird + Irs;
}

void main()
{
	vec4 lpos = view_matrix*light_position;
	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	if(color_flag == 0)				fragColor = phong(l, n, h, solid_color, 1.0);
	else if(color_flag == 1)		fragColor = texture(TEX, tc);
	else if(color_flag == 2)
	{
		fragColor = texture(TEX, tc);
		fragColor = phong(l, n, h, fragColor, 1.0);
	}
}