// input attributes of vertices
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// outputs of vertex shader = input to fragment shader
out vec4 epos;
out vec3 norm;
out vec2 tc;

// uniform variables
uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	view_matrix;
//uniform mat4	aspect_matrix;	// tricky 4x4 aspect-correction matrix
uniform mat4	projection_matrix;

uniform int		show_flag;

// shadowing...
out vec4		shadowCoord;
uniform mat4	lightMatrix;

void main()
{
	shadowCoord = lightMatrix * model_matrix * vec4(position, 1);

	if(show_flag == 0)
	{
		vec4 wpos = model_matrix  * vec4(position,1);
		epos = view_matrix* wpos;
		gl_Position = projection_matrix * epos;

		// pass eye-space normal and tc to fragment shader
		norm = normalize(mat3(view_matrix * model_matrix)*normal);
		tc = texcoord;
	}
	else if(show_flag == 1)
	{
		gl_Position = vec4(position, 1);
		tc = texcoord;
	}
	else if(show_flag == 2)
	{
		gl_Position =model_matrix* vec4(position, 1);
		tc = texcoord;
	}
}