// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

out vec2 tc;	// texture coordinate

// uniforms
uniform mat4	model_matrix;	// 4x4 transformation matrix
uniform mat4	aspect_matrix;	// tricky 4x4 aspect-correction matrix
uniform mat4	view_matrix;
uniform mat4	projection_matrix;
uniform int		mode;

void main()
{
	
	if(mode==1){
		vec4 wpos = model_matrix * vec4(position,1);
		vec4 epos = view_matrix * wpos;
		gl_Position = projection_matrix * epos;
	}else{
		gl_Position = aspect_matrix*model_matrix*vec4(position,1);
	}
	tc = texcoord;

}