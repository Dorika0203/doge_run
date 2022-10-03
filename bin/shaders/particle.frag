#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec2 tc;

// the only output variable
out vec4 fragColor;

// texture sampler
uniform sampler2D	TEX_STAR;
uniform sampler2D	TEX_FIRE;
uniform vec4		color;
uniform int		mode;


void main()
{	if(mode==0){
		fragColor = texture( TEX_STAR, tc ); if(fragColor.a < 0.001) discard;}
	else if(mode==1){
		fragColor = texture( TEX_FIRE, tc ); if(fragColor.a < 0.001) discard;}

	fragColor = vec4(fragColor.rgb,fragColor.r)*color; // enable alpha blending
}
