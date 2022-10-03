// input attributes of vertices
layout(location=0) in vec3 position;

uniform mat4 model_matrix;
uniform mat4 lightMatrix;

void main()
{
	gl_Position = lightMatrix * model_matrix * vec4(position, 1);
}