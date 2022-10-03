#ifndef __SPHERE_H__
#define __SPHERE_H__

struct sphere_t
{
	vec3	center = vec3(0,0,0);												// 3D position for translation
	float	radius = 1.0f;														// radius
	vec4	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);								// RGBA color in [0,1]
	mat4	model_matrix = mat4::identity();									// modeling transformation

	// public functions
	void	update(float t);
};

void sphere_t::update(float t)
{
	model_matrix =
	{
		radius, 0, 0, center.x,
		0, radius, 0, center.y,
		0, 0, radius, center.z,
		0, 0, 0, 1
	};
}

#endif
